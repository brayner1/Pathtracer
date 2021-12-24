#pragma once
#include <optional>

namespace Renderer
{
	class Scene;
	struct RenderOptions;

	struct SDLInputOptions
	{
		uint32_t width = 512, height = 512;
		uint32_t samplesPerPixel = 256;
		uint32_t maxDepth = 8;
		float gamma = 2.2f;
		bool useDenoiser = false;
		std::string outputFileName = "output";
	};

	class SDLLoader
	{
	public:
		SDLLoader() = delete;

		static std::optional<SDLInputOptions> SDL_LoadScene(std::string file_path, Scene& outScene);
	};
}
