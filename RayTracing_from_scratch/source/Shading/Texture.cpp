#include "pch.h"
#include "Shading\Texture.h"
#include "RenderHeaders.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Contrib/stb_image.h"

using namespace Renderer;;

Texture::Texture()
{
}

Texture::Texture(unsigned int Width, unsigned int Height, unsigned int Channels)
{
	this->data = new unsigned int[Width * Height * Channels];
}

Eigen::Vector3i Texture::getColor(uint32_t x, uint32_t y)
{
	uint32_t index = (y * this->w + x) * 3;
	if (index > this->w * this->h * 3) {
		std::cout << "texture index out of range" << std::endl;
		return Eigen::Vector3i::Zero();
	}
	uint32_t r = this->data[index];
	uint32_t g = this->data[index+1];
	uint32_t b = this->data[index+2];
	return Eigen::Vector3i(r, g, b);
}

Eigen::Vector3i Texture::getColorUV(float u, float v)
{
	uint32_t x = uint32_t(u*this->w);
	uint32_t y = uint32_t(v*this->h);
	uint32_t index = (y * this->w + x) * 3;
	if (index > this->w * this->h * 3) {
		std::cout << "texture index out of range: " << u << ", " << v << std::endl;
		return Eigen::Vector3i::Zero();
	}
	uint32_t r = this->data[index];
	uint32_t g = this->data[index + 1];
	uint32_t b = this->data[index + 2];
	return Eigen::Vector3i(r, g, b);
}


void Texture::setTexture(unsigned int Width, unsigned int Height, unsigned int Channels, unsigned char * texData)
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

void Texture::setTexture(unsigned int Width, unsigned int Height, unsigned int Channels, unsigned int * texData)
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

Texture * Renderer::Texture::LoadTextureFromFile(std::string path)
{
	std::cout << path << std::endl;
	int w, h, c;
	unsigned char* data = stbi_load(path.data(), &w, &h, &c, 0);
	Texture* texture = new Texture();
	texture->setTexture(w, h, c, data);
	return texture;
}

void Renderer::Texture::SaveTextureOnFile(std::string path)
{
	//std::cout << path << std::endl;
	std::ofstream outfile(path);
	outfile << "P3 " << w << " " << h << " 255";
	for (uint32_t index = 0; index < w*h*c; index += 3) {
		outfile << " " << int(this->data[index]) << " " << int(this->data[index + 1]) << " " << int(this->data[index + 2]);
	}
	outfile.close();
}

Texture::~Texture()
{
	delete this->data;
}
