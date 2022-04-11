#ifndef PTracer_AssimpLoader_h
#define PTracer_AssimpLoader_h
  #include "Rendering/Scene.h"

namespace Renderer {
	class AssimpLoader
	{
	public:
		AssimpLoader() = delete;

		static void Assimp_LoadScene(std::string file_path, Scene& outScene);
	};


}

#endif
