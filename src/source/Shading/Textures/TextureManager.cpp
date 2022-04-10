#include "pch.h"
#include "Shading/Textures/TextureManager.h"
#include "Contrib/stb_image.h"
#include "Shading/Textures/RGBTexture.h"
#include <filesystem>

using namespace Renderer;

TextureManager TextureManager::instance = TextureManager{};

TextureManager& TextureManager::Get()
{
	return instance;
}

uint32_t TextureManager::LoadTexture(std::string texture_path)
{
	auto it = opened_texture_map.find(texture_path);
	if (it != opened_texture_map.end())
	{
		return it->second;
	}
	//std::cout << "Loading texture [" << textures.size() << "]: " << texture_path << std::endl;
	const uint32_t index = textures.size();
	opened_texture_map[texture_path] = index;

	//textures.push_back(std::move(RGBTexture::LoadTextureFromFile(texture_path)));
	//LoadTextureFromFile(texture_path);
	textures.push_back(RGBTexture::LoadTexturePtrFromFile(texture_path));

	return textures.size()-1;
}

const RGBTexture* TextureManager::GetTexture(uint32_t texture_index) const
{
	if (texture_index >= textures.size())
		return nullptr;

	return textures[texture_index];
}

bool TextureManager::IsTextureLoaded(std::string texture_path) const
{
	auto it = opened_texture_map.find(texture_path);
	if (it != opened_texture_map.end())
	{
		return true;
	}
	return false;
}

void TextureManager::LoadTextureFromFile(std::string path)
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
	}

	const uint32_t index = textures.size();
	opened_texture_map[path] = index;
	//textures.emplace_back(w, h, 3, data, fPath.filename().string());
}
