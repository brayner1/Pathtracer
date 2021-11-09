#include "pch.h"
#include "Rendering/RenderManager.h"
#include "RenderHeaders.h"

using namespace Renderer;

void RenderManager::SaveBufferToPPM(std::string out_file_path)
{
	std::cout << "starting to write image" << std::endl;
	std::ofstream outfile(out_file_path);
	outfile << "P3 " << this->width << " " << this->height << " 255";
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			//std::cout << "writing pixel: " << x << ", " << y << std::endl;
			outfile << " " << frameBuffer[0][x][y] << " " << frameBuffer[1][x][y] << " " << frameBuffer[2][x][y];
		}
	}
	outfile.close();


}

void RenderManager::InitializeFramebuffer()
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

	this->colorBuffer = new float[width * height * 3];
	this->albedoBuffer = new float[width * height * 3];
	this->normalBuffer = new float[width * height * 3];
}

void RenderManager::RenderScene(Scene& scene, ImageType outputType)
{
	this->InitializeFramebuffer();
#ifdef paralellism
	omp_set_num_threads(omp_get_max_threads());
	std::cout << "max threads: " << omp_get_max_threads() << std::endl;
	//std::cout << "started parallel region with " << omp_get_num_threads() << " threads" << std::endl;
	int total = width * height;
	int* it = new int; *it = 0;
	int* lastreport = new int; *lastreport = -1;

	const std::clock_t start = std::clock();

//#pragma omp parallel for shared(lastreport, it)
	#pragma omp parallel for shared(lastreport, it) schedule(dynamic, 16)
#endif // paralellism
	for (int idx = 0; idx < width * height; idx++)
	{  /// Columns
		int x = idx % height;
		int y = idx / width;
		Eigen::Vector3f finalColor = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
		Eigen::Vector3f finalAlbedo = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
		Eigen::Vector3f finalNormal = Eigen::Vector3f(0.0f, 0.0f, 0.0f);

		OutputProperties OP = OutputProperties();
		scene.PixelColor(x, y, maxDepth, this->samplesPerPixel, OP);
		finalColor = OP.Color;
		finalAlbedo = OP.Albedo;
		finalNormal = OP.Normal;

		finalColor = finalColor.cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f));

		
		//int rawIndex = (x + width / 2 - ((width + 1) % 2) + (y + height / 2 - ((height + 1) % 2)) * width) * 3;
		int rawIndex = (x + (y * width)) * 3;
		//int xIndex = x + width / 2 - 1, yIndex = y + height / 2 - 1;

#pragma omp critical
		{
			*it += 1;
			int prog = (*it)*100.0f / total;
			if ((prog % 5) == 0 && prog != *lastreport)
			{
				*lastreport = prog;
				std::cout << "Progress: " << prog << "%" << std::endl << "Pixel: " << *it << " of " << total << std::endl;
			}
			
			this->colorBuffer[rawIndex + 0] = finalColor.x();
			this->colorBuffer[rawIndex + 1] = finalColor.y();
			this->colorBuffer[rawIndex + 2] = finalColor.z();

			this->albedoBuffer[rawIndex + 0] = finalAlbedo.x();
			this->albedoBuffer[rawIndex + 1] = finalAlbedo.y();
			this->albedoBuffer[rawIndex + 2] = finalAlbedo.z();

			this->normalBuffer[rawIndex + 0] = finalNormal.x();
			this->normalBuffer[rawIndex + 1] = finalNormal.y();
			this->normalBuffer[rawIndex + 2] = finalNormal.z();
			
			frameBuffer[0][x][y] = (int)(powf(finalColor.x(), 1.0f / this->gamma) * 255.f);
			frameBuffer[1][x][y] = (int)(powf(finalColor.y(), 1.0f / this->gamma) * 255.f);
			frameBuffer[2][x][y] = (int)(powf(finalColor.z(), 1.0f / this->gamma) * 255.f);
		}
	}
	


	const float duration = (std::clock() - start) / (float)CLOCKS_PER_SEC;
	std::cout << "rendered in " << duration << " seconds" << std::endl;

	

	this->SaveBufferToPPM("noisy.ppm");

	this->ExecuteDenoiser();

	this->SaveBufferToPPM("img.ppm");


	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			frameBuffer[0][x][y] = (int)(powf((normalBuffer[(x + y * width) * 3 + 0] + 1.0f)/2.0f, 1.0f / this->gamma) * 255.f);
			frameBuffer[1][x][y] = (int)(powf((normalBuffer[(x + y * width) * 3 + 1] + 1.0f)/2.0f, 1.0f / this->gamma) * 255.f);
			frameBuffer[2][x][y] = (int)(powf((normalBuffer[(x + y * width) * 3 + 2] + 1.0f)/2.0f, 1.0f / this->gamma) * 255.f);
		}
	}
	this->SaveBufferToPPM("normal.ppm");

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			frameBuffer[0][x][y] = (int)(powf(albedoBuffer[(x + y * width) * 3 + 0], 1.0f / this->gamma) * 255.f);
			frameBuffer[1][x][y] = (int)(powf(albedoBuffer[(x + y * width) * 3 + 1], 1.0f / this->gamma) * 255.f);
			frameBuffer[2][x][y] = (int)(powf(albedoBuffer[(x + y * width) * 3 + 2], 1.0f / this->gamma) * 255.f);
		}
	}
	this->SaveBufferToPPM("albedo.ppm");
}

void Renderer::RenderManager::ExecuteDenoiser()
{
	float* output = new float[width * height * 3];

	oidn::DeviceRef device = oidn::newDevice(oidn::DeviceType::Default);
	device.commit();

	oidn::FilterRef filter = device.newFilter("RT");
	filter.setImage("color", this->colorBuffer, oidn::Format::Float3, width, height);
	filter.setImage("albedo", this->albedoBuffer, oidn::Format::Float3, width, height);
	filter.setImage("normal", this->normalBuffer, oidn::Format::Float3, width, height);
	filter.setImage("output", output, oidn::Format::Float3, width, height);

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
			frameBuffer[0][x][y] = (int)(powf(output[(x + y * width) * 3 + 0], 1.0f / this->gamma) * 255.f);
			frameBuffer[1][x][y] = (int)(powf(output[(x + y * width) * 3 + 1], 1.0f / this->gamma) * 255.f);
			frameBuffer[2][x][y] = (int)(powf(output[(x + y * width) * 3 + 2], 1.0f / this->gamma) * 255.f);
		}
	}
}

RenderManager::RenderManager(int width, int height, float horizontal_field_of_view)
{
	this->width = width; this->height = height;
	this->gamma = 2.2f;
	this->screen_aspect_ratio = ((float)this->width) / ((float)this->height);
	this->horizontal_fov = horizontal_field_of_view;
	this->vertical_fov = this->horizontal_fov / this->screen_aspect_ratio;
	this->samplesPerPixel = 32;
	this->maxDepth = 8;
}


