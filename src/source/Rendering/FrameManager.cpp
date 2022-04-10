#include "pch.h"
#include "Rendering/FrameManager.h"
#include "RenderHeaders.h"
#include <chrono>

using namespace Renderer;

FrameManager::FrameManager(int width, int height) : width(width), height(height)
{
}

FrameManager::FrameManager(const RenderOptions& renderOptions)
{
	SetOptions(renderOptions);
}

void FrameManager::SetOptions(const RenderOptions& renderOptions)
{
	if (renderOptions.width)
		this->width = renderOptions.width.value();
	if (renderOptions.height)
		this->height = renderOptions.height.value();
	if (renderOptions.samplesPerPixel)
		this->samplesPerPixel = renderOptions.samplesPerPixel.value();
	if (renderOptions.maxDepth)
		this->maxDepth = renderOptions.maxDepth.value();
	if (renderOptions.gamma)
		this->gamma = renderOptions.gamma.value();
	if (renderOptions.exposure)
		this->exposure = renderOptions.exposure.value();
	if (renderOptions.useDenoiser)
		this->useDenoiser = renderOptions.useDenoiser.value();
	if (renderOptions.outputFileName)
		this->outFile = renderOptions.outputFileName.value();

	frameBufferInitialized = false;
}

void FrameManager::InitializeFramebuffer()
{
	for (size_t i = 0; i < 3; i++)
	{
		frameBuffer[i] = new int* [this->width];
		for (size_t j = 0; j < width; j++)
		{
			frameBuffer[i][j] = new int[this->height];
			for (size_t k = 0; k < height; k++)
			{
				frameBuffer[i][j][k] = 0;
			}
		}
	}

	if (useDenoiser)
	{
		this->colorBuffer = new float[width * height * 3];
		this->albedoBuffer = new float[width * height * 3];
		this->normalBuffer = new float[width * height * 3];
	}

	frameBufferInitialized = true;
}

void FrameManager::SaveBufferToPPM(std::string out_file_path)
{
	std::cout << "starting to write image" << std::endl;
	std::ofstream outfile(out_file_path);
	outfile << "P3 " << this->width << " " << this->height << " 255";
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			//std::cout << "writing pixel: " << x << ", " << y << std::endl;
			Eigen::Vector3i color { frameBuffer[0][x][y], frameBuffer[1][x][y], frameBuffer[2][x][y] };
			color.cwiseMin(Eigen::Vector3i::Ones()).cwiseMax(Eigen::Vector3i::Zero());
			//outfile << " " << frameBuffer[0][x][y] << " " << frameBuffer[1][x][y] << " " << frameBuffer[2][x][y];
			outfile << " " << color.x() << " " << color.y() << " " << color.z();
		}
	}
	outfile.close();
}

void FrameManager::RenderScene(Scene& scene, ImageType outputType)
{
	if (!frameBufferInitialized)
		this->InitializeFramebuffer();

	const PinholeCamera& camera = scene.scene_camera;
	int width = camera.GetWidth(), height = camera.GetHeight();
	int total = width * height;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#ifdef paralellism
	omp_set_num_threads(omp_get_max_threads());
	std::cout << "Number of threads available: " << omp_get_max_threads() << std::endl;

	// Report variables
	int* it = new int; *it = 0;
	int* lastreport = new int; *lastreport = -1;

//#pragma omp parallel for shared(lastreport, it)
	#pragma omp parallel for shared(lastreport, it) schedule(dynamic, 16)
#endif // paralellism
	for (int idx = 0; idx < total; idx++)
	{  
		int x = idx % width;
		int y = idx / width;
		Eigen::Vector3f finalColor = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
		Eigen::Vector3f finalAlbedo = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
		Eigen::Vector3f finalNormal = Eigen::Vector3f(0.0f, 0.0f, 0.0f);

		OutputProperties OP = OutputProperties();
		scene.PixelColor(x, y, maxDepth, samplesPerPixel, OP);

		finalColor = OP.color;
		if (exposure > 0.f)
			finalColor = Eigen::Array3f::Ones() - exp((-finalColor * exposure).array());
		finalAlbedo = OP.albedo;
		finalNormal = OP.normal;

		int rawIndex = (x + (y * width)) * 3;

		// Report the progress at each 5% mark of the image rendered
#ifdef paralellism
#pragma omp critical
#endif
		{
			*it += 1;
			int prog = (*it)*100.0f / total;
			if ((prog % 5) == 0 && prog != *lastreport)
			{
				*lastreport = prog;
				std::cout << "Progress: " << prog << "%" << std::endl << "Pixel: " << *it << " of " << total << std::endl;
			}

			if (useDenoiser)
			{
				this->colorBuffer[rawIndex + 0] = finalColor.x();
				this->colorBuffer[rawIndex + 1] = finalColor.y();
				this->colorBuffer[rawIndex + 2] = finalColor.z();

				this->albedoBuffer[rawIndex + 0] = finalAlbedo.x();
				this->albedoBuffer[rawIndex + 1] = finalAlbedo.y();
				this->albedoBuffer[rawIndex + 2] = finalAlbedo.z();

				this->normalBuffer[rawIndex + 0] = finalNormal.x();
				this->normalBuffer[rawIndex + 1] = finalNormal.y();
				this->normalBuffer[rawIndex + 2] = finalNormal.z();
			}

			const Eigen::Vector3f gammaCorrectedColor = pow(finalColor.array(), 1.0f / this->gamma);

			frameBuffer[0][x][y] = (int)(gammaCorrectedColor.x() * 255.f);
			frameBuffer[1][x][y] = (int)(gammaCorrectedColor.y() * 255.f);
			frameBuffer[2][x][y] = (int)(gammaCorrectedColor.z() * 255.f);
		}
	}
	

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Rendering time = " << (float)std::chrono::duration_cast<
				std::chrono::microseconds>(end - begin).count() / 1000000.f << "[s]" << std::endl;

	this->SaveBufferToPPM(outFile+"_noisy.ppm");

	if (!useDenoiser)
		return;

	this->ExecuteDenoiser();

	this->SaveBufferToPPM(outFile+"_denoised.ppm");


	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			frameBuffer[0][x][y] = (int)(powf((normalBuffer[(x + y * width) * 3 + 0] + 1.0f)/2.0f, 1.0f / this->gamma) * 255.f);
			frameBuffer[1][x][y] = (int)(powf((normalBuffer[(x + y * width) * 3 + 1] + 1.0f)/2.0f, 1.0f / this->gamma) * 255.f);
			frameBuffer[2][x][y] = (int)(powf((normalBuffer[(x + y * width) * 3 + 2] + 1.0f)/2.0f, 1.0f / this->gamma) * 255.f);
		}
	}
	this->SaveBufferToPPM(outFile+"_normals.ppm");

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			frameBuffer[0][x][y] = (int)(powf(albedoBuffer[(x + y * width) * 3 + 0], 1.0f / this->gamma) * 255.f);
			frameBuffer[1][x][y] = (int)(powf(albedoBuffer[(x + y * width) * 3 + 1], 1.0f / this->gamma) * 255.f);
			frameBuffer[2][x][y] = (int)(powf(albedoBuffer[(x + y * width) * 3 + 2], 1.0f / this->gamma) * 255.f);
		}
	}
	this->SaveBufferToPPM(outFile+"_albedo.ppm");
}

void FrameManager::ExecuteDenoiser()
{
	float* output = new float[width * height * 3];

	oidn::DeviceRef device = oidn::newDevice(oidn::DeviceType::Default);
	device.commit();

	oidn::FilterRef filter = device.newFilter("RT");
	filter.setImage("color", this->colorBuffer, oidn::Format::Float3, width, height);
	filter.setImage("albedo", this->albedoBuffer, oidn::Format::Float3, width, height);
	filter.setImage("normal", this->normalBuffer, oidn::Format::Float3, width, height);
	filter.setImage("output", output, oidn::Format::Float3, width, height);

	filter.set("cleanAux", true);

	filter.commit();

	filter.execute();

	const char* errorMessage;
	if (device.getError(errorMessage) != oidn::Error::None)
		std::cout << "Error: " << errorMessage << std::endl;
	else
		std::cout << "No error!" << std::endl;
	
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			frameBuffer[0][x][y] = (int)(pow(output[(x + y * width) * 3 + 0], 1.0f / this->gamma) * 255.f);
			frameBuffer[1][x][y] = (int)(pow(output[(x + y * width) * 3 + 1], 1.0f / this->gamma) * 255.f);
			frameBuffer[2][x][y] = (int)(pow(output[(x + y * width) * 3 + 2], 1.0f / this->gamma) * 255.f);
		}
	}
}




