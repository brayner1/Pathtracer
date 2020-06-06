#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

namespace Renderer {
	class Texture
	{
	private:
		unsigned int* data;
		unsigned int w, h, c;
	public:
		Texture();
		Texture(unsigned int Width, unsigned int Height, unsigned int Channels);

		Eigen::Vector3i getColor(uint32_t x, uint32_t y);
		Eigen::Vector3i getColorUV(float u, float v);
		//glm::uvec4 getColor(uint32_t x, uint32_t y);


		void setTexture(unsigned int Width, unsigned int Height, unsigned int Channels, unsigned char* texData);
		void setTexture(unsigned int Width, unsigned int Height, unsigned int Channels, unsigned int* texData);

		static Texture* LoadTextureFromFile(std::string path);
		void SaveTextureOnFile(std::string path);

		~Texture();
	};
}

#endif