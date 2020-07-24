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
	int total = width * height;
	int* it = new int; *it = 0;
	int* lastreport = new int; *lastreport = -1;

	

#pragma omp parallel for shared(lastreport, it)
#endif // paralellism
	for (int y = height / 2; y >= -(height / 2) + ((height + 1) % 2); --y) 
	{    /// Rows
#ifdef paralellism
//#pragma omp parallel for shared(lastreport, it)
#endif // paralellism
		for (int x = -(width / 2) + ((width + 1) % 2); x <= width / 2; ++x) 
		{  /// Columns
			Eigen::Vector4f finalColor = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
			Eigen::Vector4f finalAlbedo = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
			Eigen::Vector4f finalNormal = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);

			OutputProperties OP = OutputProperties();
			scene.PixelColor(x, y, maxDepth, this->samplesPerPixel, OP);
			finalColor = OP.Color;
			finalAlbedo = OP.Albedo;
			finalNormal = OP.Normal;

			finalColor = finalColor.cwiseMin(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 0.0f));

			
			int rawIndex = (x + width / 2 - ((width + 1) % 2) + (y + height / 2 - ((height + 1) % 2)) * width) * 3;
			int xIndex = x + width / 2 - 1, yIndex = y + height / 2 - 1;

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
				
				frameBuffer[0][xIndex][yIndex] = (int)(powf(finalColor.x(), 1.0f / this->gamma) * 255.f);
				frameBuffer[1][xIndex][yIndex] = (int)(powf(finalColor.y(), 1.0f / this->gamma) * 255.f);
				frameBuffer[2][xIndex][yIndex] = (int)(powf(finalColor.z(), 1.0f / this->gamma) * 255.f);
			}
		}
	}




	

	this->SaveBufferToPPM("noisy.ppm");

	this->ExecuteDenoiser();

	this->SaveBufferToPPM("img.ppm");


	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			frameBuffer[0][x][y] = (int)(powf((normalBuffer[(x + y * width) * 3 + 0] + 1.0f)/2.0f, 1.0f / this->gamma) * 255.f);
			frameBuffer[1][x][y] = (int)(powf((normalBuffer[(x + y * width) * 3 + 1] + 1.0f)/2.0f, 1.0f / this->gamma) * 255.f);
			frameBuffer[2][x][y] = (int)(powf((normalBuffer[(x + y * width) * 3 + 2] + 1.0f)/2.0f, 1.0f / this->gamma) * 255.f);
		}
	}
	this->SaveBufferToPPM("normal.ppm");

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
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
	this->gamma = 2.2f;
	this->screen_aspect_ratio = ((float)this->width) / ((float)this->height);
	this->horizontal_fov = horizontal_field_of_view;
	this->vertical_fov = this->horizontal_fov / this->screen_aspect_ratio;
	this->samplesPerPixel = 20;
	this->maxDepth = 8;
}


