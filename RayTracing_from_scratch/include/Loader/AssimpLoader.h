#pragma once
#include "Rendering/Scene.h"

namespace Renderer {
	class AssimpLoader
	{
	public:
		AssimpLoader() = delete;

		static void Assimp_LoadScene(std::string file_path, Scene& outScene);
	};


}
