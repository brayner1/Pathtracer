#include "pch.h"
#include "Rendering/Integrator/Integrator.h"
#include "RenderHeaders.h"

void Renderer::Integrator::SaveBufferToPPM(std::string out_file_path)
{
	std::ofstream outfile(out_file_path);
	outfile << "P3 " << this->Width << " " << this->Height << " 255";
	std::cout << "starting to write image" << std::endl;
	for (int y = Height - 1; y >= 0; y--)
	{
		for (int x = Width - 1; x >= 0; x--)
		{
			//std::cout << "writing pixel: " << x << ", " << y << std::endl;
			outfile << " " << frameBuffer[0][x][y] << " " << frameBuffer[1][x][y] << " " << frameBuffer[2][x][y];
		}
	}
	outfile.close();
}

void Renderer::Integrator::InitializeFramebuffer()
{
	int w = Width; int h = Height;
	for (size_t i = 0; i < 3; i++)
	{
		frameBuffer[i] = new int* [w];
		for (size_t j = 0; j < w; j++)
		{
			frameBuffer[i][j] = new int[h];
			for (size_t k = 0; k < h; k++)
			{
				frameBuffer[i][j][k] = 0;
			}
		}
	}

	this->colorBuffer = new float[w * h * 3];
	this->albedoBuffer = new float[w * h * 3];
	this->normalBuffer = new float[w * h * 3];
}

