#include "pch.h"
#include "Rendering/RenderManager.h"
#include "RenderHeaders.h"

using namespace Renderer;

void RenderManager::SaveBufferToPPM(std::string out_file_path)
{
	std::ofstream outfile(out_file_path);
	outfile << "P3 " << this->width << " " << this->height << " 255";
	std::cout << "starting to write image" << std::endl;
	for (int y = height - 1; y >= 0; y--)
	{
		for (int x = width - 1; x >= 0; x--)
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

void RenderManager::RenderScene(Scene scene, ImageType outputType)
{
	this->InitializeFramebuffer();
#ifdef paralellism
	omp_set_num_threads(omp_get_max_threads());
	std::cout << "max threads: " << omp_get_max_threads() << std::endl;
	//std::cout << "started parallel region with " << omp_get_num_threads() << " threads" << std::endl;
#pragma omp parallel for
#endif // paralellism
	for (int y = height / 2; y >= -(height / 2) + ((height + 1) % 2); --y) 
	{    /// Rows
#ifdef paralellism
#pragma omp parallel for
#endif // paralellism
		for (int x = -(width / 2) + ((width + 1) % 2); x <= width / 2; ++x) 
		{  /// Columns
			Eigen::Vector3f finalColor = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
			Eigen::Vector3f finalAlbedo = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
			Eigen::Vector3f finalNormal = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
			for (int i = 0; i < this->samplesPerPixel; i++) 
			{
				OutputProperties OP = OutputProperties();
				scene.getPixelColor(x, y, maxDepth, OP);
				finalColor += OP.Color;
				finalAlbedo = OP.Albedo;
				finalNormal = OP.Normal;
			}
			finalColor = (finalColor / (float)(samplesPerPixel)).cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f));

			
			

#pragma omp critical
			{
				this->colorBuffer[(x + width / 2 - ((width + 1) % 2) + (y + height / 2 - ((height + 1) % 2)) * width) * 3 + 0] = finalColor.x();
				this->colorBuffer[(x + width / 2 - ((width + 1) % 2) + (y + height / 2 - ((height + 1) % 2)) * width) * 3 + 1] = finalColor.y();
				this->colorBuffer[(x + width / 2 - ((width + 1) % 2) + (y + height / 2 - ((height + 1) % 2)) * width) * 3 + 2] = finalColor.z();

				this->albedoBuffer[(x + width / 2 - ((width + 1) % 2) + (y + height / 2 - ((height + 1) % 2)) * width) * 3 + 0] = finalAlbedo.x();
				this->albedoBuffer[(x + width / 2 - ((width + 1) % 2) + (y + height / 2 - ((height + 1) % 2)) * width) * 3 + 1] = finalAlbedo.y();
				this->albedoBuffer[(x + width / 2 - ((width + 1) % 2) + (y + height / 2 - ((height + 1) % 2)) * width) * 3 + 2] = finalAlbedo.z();

				this->normalBuffer[(x + width / 2 - ((width + 1) % 2) + (y + height / 2 - ((height + 1) % 2)) * width) * 3 + 0] = finalNormal.x();
				this->normalBuffer[(x + width / 2 - ((width + 1) % 2) + (y + height / 2 - ((height + 1) % 2)) * width) * 3 + 1] = finalNormal.y();
				this->normalBuffer[(x + width / 2 - ((width + 1) % 2) + (y + height / 2 - ((height + 1) % 2)) * width) * 3 + 2] = finalNormal.z();
			frameBuffer[0][x + width / 2 - 1][y + height / 2 - 1] = (int)(powf(finalColor.x(), 1.0f / this->gamma) * 255.f);
			frameBuffer[1][x + width / 2 - 1][y + height / 2 - 1] = (int)(powf(finalColor.y(), 1.0f / this->gamma) * 255.f);
			frameBuffer[2][x + width / 2 - 1][y + height / 2 - 1] = (int)(powf(finalColor.z(), 1.0f / this->gamma) * 255.f);
			}
		}
	}

	//for (int x = 0; x < width; x++)
	//{
	//	for (int y = 0; y < height; y++)
	//	{
	//		frameBuffer[0][x][y] = (int)(powf(normalBuffer[(x + y * width)*3 + 0], 1.0f / this->gamma) * 255.f);
	//		frameBuffer[1][x][y] = (int)(powf(normalBuffer[(x + y * width)*3 + 1], 1.0f / this->gamma) * 255.f);
	//		frameBuffer[2][x][y] = (int)(powf(normalBuffer[(x + y * width)*3 + 2], 1.0f / this->gamma) * 255.f);
	//	}
	//}


	

	this->SaveBufferToPPM("noisy.ppm");

	this->ExecuteDenoiser();

	this->SaveBufferToPPM("img.ppm");
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

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
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
	this->gamma = 1.0f;
	this->screen_aspect_ratio = ((float)this->width) / ((float)this->height);
	this->horizontal_fov = horizontal_field_of_view;
	this->vertical_fov = this->horizontal_fov / this->screen_aspect_ratio;
	this->samplesPerPixel = 20;
	this->maxDepth = 8;
}


