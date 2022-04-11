#ifndef PTracer_TextureManager_h
#define PTracer_TextureManager_h
#include <unordered_map>
#include <vector>
#include "RGBTexture.h"

namespace Renderer
{
	class RGBTexture;

	// Singleton for managing texture loading
	class TextureManager
	{
	public:
		static TextureManager& Get();

		~TextureManager() { std::cout << "deleting texture manager.\n"; }

		uint32_t LoadTexture(std::string texture_path);

		[[nodiscard]]
		const RGBTexture* GetTexture(uint32_t texture_index) const;

		[[nodiscard]]
		bool IsTextureLoaded(std::string texture_path) const;

		[[nodiscard]]
		uint32_t GetNumberOfTexturesLoaded() const
		{
			return textures.size();
		}

	private:
		TextureManager() { std::cout << "Creating Texture Manager\n"; }
		static TextureManager instance;

		void LoadTextureFromFile(std::string path);

		std::vector<RGBTexture*> textures {};
		std::unordered_map<std::string, uint32_t> opened_texture_map {};
	};
}


#endif
