#include "pch.h"
#include "Shading\RGBTexture.h"
#include "RenderHeaders.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Contrib/stb_image.h"

using namespace Renderer;;

RGBTexture::RGBTexture()
{
}

RGBTexture::RGBTexture(unsigned int Width, unsigned int Height, unsigned int Channels)
{
	this->data = new unsigned int[Width * Height * Channels];
}

RGBTexture::~RGBTexture()
{
	delete this->data;
}

Eigen::Vector3i RGBTexture::GetColor(uint32_t x, uint32_t y)
{
	uint32_t index = (y * this->w + x) * 3;
	if (index > this->w * this->h * 3) {
		std::cout << "Error: texture index (" << x << ", " << y  << ") out of range" << std::endl;
		std::cout << "Image Size: " << this->w << " x " << this->h << std::endl;
		return Eigen::Vector3i::Zero();
	}
	uint32_t r = this->data[index];
	uint32_t g = this->data[index+1];
	uint32_t b = this->data[index+2];
	return Eigen::Vector3i(r, g, b);
}

Eigen::Vector3i RGBTexture::GetColorUV(float u, float v)
{
	/*if (u < 0 || v < 0)
	{
		std::cout << "uv values should be between [0, 1]. Passed Values: " << u << ", " << v << "\n";
		return Eigen::Vector3i::Zero();
	}*/
	u = (u > 1)? u - int(u) : (u < 0)? u - int(u-1) : u;
	v = (v > 1)? v - int(v) : (v < 0)? v - int(v-1) : v;
	uint32_t x = uint32_t(floorf(u*(this->w-1)));
	uint32_t y = uint32_t(floorf((1.0f - v)*(this->h-1)));
	return GetColor(x, y);
}


void RGBTexture::SetTexture(unsigned int Width, unsigned int Height, unsigned int Channels, unsigned char * texData)
{
	this->w = Width;
	this->h = Height;
	this->c = Channels;
	this->data = new unsigned int[Width * Height * Channels];
	for (size_t i = 0; i < Width * Height; i++)
	{
		size_t index = i * Channels;
		for (size_t c = 0; c < Channels; c++)
		{
			this->data[index + c] = uint32_t(texData[index + c]);
		}
	}
}

void RGBTexture::SetTexture(unsigned int Width, unsigned int Height, unsigned int Channels, unsigned int * texData)
{
	this->w = Width;
	this->h = Height;
	this->c = Channels;
	this->data = new unsigned int[Width * Height * Channels];
	for (size_t i = 0; i < Width * Height; i++)
	{
		size_t index = i * Channels;
		for (size_t c = 0; c < Channels; c++)
		{
			this->data[index + c] = texData[index + c];
		}
	}
}

RGBTexture * Renderer::RGBTexture::LoadTextureFromFile(std::string path)
{
	std::cout << path << std::endl;
	int w, h, c;
	unsigned char* data = stbi_load(path.data(), &w, &h, &c, 0);
	if (!data) 
	{ 
		std::cout << "Failed to load texture\n";
		return nullptr; 
	}
	RGBTexture* texture = new RGBTexture();
	texture->SetTexture(w, h, c, data);
	return texture;
}

void Renderer::RGBTexture::SaveTextureOnFile(std::string path)
{
	//std::cout << path << std::endl;
	std::ofstream outfile(path);
	outfile << "P3 " << w << " " << h << " 255";
	for (uint32_t index = 0; index < w*h*c; index += 3) {
		outfile << " " << int(this->data[index]) << " " << int(this->data[index + 1]) << " " << int(this->data[index + 2]);
	}
	outfile.close();
}


