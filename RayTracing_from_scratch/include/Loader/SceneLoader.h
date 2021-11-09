#pragma once
#include "Rendering/Scene.h"

namespace Renderer {
	class SceneLoader
	{
	private:
		Assimp::Importer importer;

		Scene renderer_scene;
		const aiScene* assimp_scene;

		Eigen::AlignedBox3f get_bounding_box_for_node(const aiNode* nd,
			aiMatrix4x4t<float> trafo,
			const Eigen::AlignedBox3f* const previous_boundingBox
		);

		Eigen::AlignedBox3f SceneLoader::get_bounding_box_for_mesh(const aiMesh* mesh);

		Eigen::Vector3f convert_assimp_color(const aiColor4D color) {
			return Eigen::Vector3f(color.r, color.g, color.b);
		}

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

			SceneLoader(std::string file_path);

		void convertAssimpScene();
		const Scene getRendererScene();

		~SceneLoader();
	};


}
