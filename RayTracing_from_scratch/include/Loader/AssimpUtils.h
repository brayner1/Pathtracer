#pragma once
#include <iosfwd>
#include <vector>

#include "Object/Object.h"

class aiScene;

namespace Renderer
{
	class Scene;
	class RGBTexture;
	class Material;

	Material* ConvertAssimpMaterial(const aiScene* assimpScene, const aiMaterial* material);

	RGBTexture* LoadTexture(const aiScene* assimpScene, const aiMaterial* material, aiTextureType type, uint32_t index);


	/**
	 * \brief This function takes a pointer to an aiScene and inserts the meshes of that scene into the Renderer Scene reference passed.
	 * \param assimpScene The pointer to the Assimp aiScene that will be converted.
	 * \param outRendererScene A reference to the Renderer Scene that will have the converted aiScene primitives.
	 * \param overrideMaterial A pointer to an override material. If nullptr, will use the materials defined in the imported file.
	 * \return A vector with the pointers to the objects inserted in the Scene variable.
	 */
	std::vector<Object*> ConvertAssimpScene(const aiScene* assimpScene, Scene& outRendererScene,
	                                        Material* overrideMaterial = nullptr);


	/**
	 * \brief Converts the input assimp color structure to the Renderer default Eigen Vector
	 * \param color 
	 * \return 
	 */
	inline Eigen::Vector3f ConvertAssimpColor(const aiColor4D color)
	{
		return Eigen::Vector3f(color.r, color.g, color.b);
	}

	/**
	 * \brief Get the axis-aligned bounding box of the passed assimp mesh
	 * \param mesh 
	 * \return 
	 */
	inline Eigen::AlignedBox3f GetMeshBounds(const aiMesh* mesh)
	{
		unsigned int n = 0;

		Eigen::Vector3f min = { FLT_MAX, FLT_MAX, FLT_MAX }, max = {-FLT_MAX, - FLT_MAX, - FLT_MAX};
		for (unsigned int t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];
			//aiTransformVecByMatrix4(&tmp, trafo);
			min(0) = (min.x() < tmp.x) ? min.x() : tmp.x;
			min(1) = (min.y() < tmp.y) ? min.y() : tmp.y;
			min(2) = (min.z() < tmp.z) ? min.z() : tmp.z;

			max(0) = (max.x() > tmp.x) ? max.x() : tmp.x;
			max(1) = (max.y() > tmp.y) ? max.y() : tmp.y;
			max(2) = (max.z() > tmp.z) ? max.z() : tmp.z;
		}
		Eigen::AlignedBox3f boundingBox = Eigen::AlignedBox3f(min, max);

		return boundingBox;
	}

}