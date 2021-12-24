#include "pch.h"
#include "Loader/AssimpLoader.h"
#include "Loader/AssimpUtils.h"
#include "RenderHeaders.h"
//#include "Object/Mesh.h"
using namespace Renderer;

void AssimpLoader::Assimp_LoadScene(std::string file_path, Scene& outScene)
{
	Assimp::Importer assimpImporter;
	const aiScene* assimpScene = assimpImporter.ReadFile(file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	if (!assimpScene) {
		std::cout << "Assimp Error on load:\n" << assimpImporter.GetErrorString() << std::endl;
	}

	ConvertAssimpScene(assimpScene, outScene);
}
