#include "pch.h"
#include "Shading/Textures/RGBTexture.h"
#include "RenderHeaders.h"
#define STB_IMAGE_IMPLEMENTATION
#include <filesystem>

#include "Contrib/stb_image.h"

using namespace Renderer;;

int RGBTexture::numTextures = 0;

RGBTexture::RGBTexture()
{
	w = h = c = 0;
	texture_file = "Texture" + std::to_string(numTextures++);
}

RGBTexture::RGBTexture(unsigned int Width, unsigned int Height, unsigned int Channels, unsigned char* data, std::string texture_name) : texture_file(texture_name)
{
	SetTexture(Width, Height, Channels, data);
	if (texture_name == "")
		texture_file = "Texture" + std::to_string(numTextures++);
	//std::cout << "Creating texture " << texture_file << std::endl;
}

RGBTexture::RGBTexture(RGBTexture&& other) noexcept
{
	w = other.w;
	h = other.h;
	c = other.c;
	data = other.data;
	other.data = nullptr;
	if (data == nullptr)
		std::cout << "tex " << other.texture_file << " has null data\n";
	if (other.texture_file != "")
		texture_file = other.texture_file;
	else
		texture_file = "Texture" + std::to_string(numTextures++);

	//std::cout << "Moving texture " << texture_file << std::endl;
}

RGBTexture::~RGBTexture()
{
	//std::cout << "deleting texture " << texture_file << std::endl;
	delete this->data;
}

Eigen::Vector3i RGBTexture::GetColor(uint32_t x, uint32_t y) const
{
	uint32_t index = (y * this->w + x) * 3;
	if (index > this->w * this->h * 3) {
		std::cout << "Error: texture index (" << x << ", " << y  << ") out of range" << std::endl;
		std::cout << "Texture Name: " << texture_file << std::endl;
		return Eigen::Vector3i::Zero();
	}
	uint32_t r = this->data[index];
	uint32_t g = this->data[index+1];
	uint32_t b = this->data[index+2];
	return Eigen::Vector3i(r, g, b);
}

Eigen::Vector3i RGBTexture::GetColorUV(float u, float v) const
{
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
	this->data = new unsigned char[Width * Height * Channels];
	memcpy(this->data, texData, w * h * c);
}

RGBTexture* RGBTexture::LoadTexturePtrFromFile(std::string path)
{
	std::filesystem::path fPath {path};
	if (stbi_is_hdr(path.c_str()))
	{
		std::cout << path << " is hdr\n";
	}

	if (stbi_is_16_bit(path.c_str()))
	{
		std::cout << path << " is 16-bit\n";
	}

	int w, h, c;
	unsigned char* data = stbi_load(path.data(), &w, &h, &c, 3);
	if (!data) 
	{ 
		std::cout << "Failed to load texture:\n" << path << "\n";
		return nullptr; 
	}
	

	RGBTexture* texture = new RGBTexture(w, h, 3, data, fPath.filename().string());
	//texture->SetTexture(w, h, 3, data);
	return texture;
}

RGBTexture RGBTexture::LoadTextureFromFile(std::string path)
{
	std::filesystem::path fPath {path};
	if (stbi_is_hdr(path.c_str()))
	{
		std::cout << path << " is hdr\n";
	}

	if (stbi_is_16_bit(path.c_str()))
	{
		std::cout << path << " is 16-bit\n";
	}

	int w, h, c;
	unsigned char* data = stbi_load(path.data(), &w, &h, &c, 3);
	if (!data) 
	{ 
		std::cout << "Failed to load texture:\n" << path << "\n";
		return RGBTexture{};
	}

	return std::move(RGBTexture{ static_cast<uint32_t>(w), static_cast<uint32_t>(h), 3, data, fPath.filename().string() });
}

void RGBTexture::SaveTextureOnFile(std::string path) const
{
	//std::cout << path << std::endl;
	std::ofstream outfile(path);
	outfile << "P3 " << w << " " << h << " 255";
	for (uint32_t index = 0; index < w*h*c; index += 3) {
		outfile << " " << int(this->data[index]) << " " << int(this->data[index + 1]) << " " << int(this->data[index + 2]);
	}
	outfile.close();
}


