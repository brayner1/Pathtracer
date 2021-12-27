#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

namespace Renderer {
	class RGBTexture
	{
	private:
		unsigned int* data;
		unsigned int w, h, c;
	public:
		RGBTexture();
		RGBTexture(unsigned int Width, unsigned int Height, unsigned int Channels);

		~RGBTexture();

		Eigen::Vector3i GetColor(uint32_t x, uint32_t y);
		Eigen::Vector3i GetColorUV(float u, float v);


		void SetTexture(unsigned int Width, unsigned int Height, unsigned int Channels, unsigned char* texData);
		void SetTexture(unsigned int Width, unsigned int Height, unsigned int Channels, unsigned int* texData);

		static RGBTexture* LoadTextureFromFile(std::string path);
		void SaveTextureOnFile(std::string path);
	};
}

#endif