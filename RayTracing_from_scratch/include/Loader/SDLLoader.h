#pragma once
#include <optional>

namespace Renderer
{
	class Scene;
	struct RenderOptions;

	class SDLLoader
	{
	public:
		SDLLoader() = delete;

		static std::optional<RenderOptions> SDL_LoadScene(std::string file_path, Scene& outScene);
	};
}
