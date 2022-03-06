#include "pch.h"
#include "Loader/AssimpUtils.h"
#include "RenderHeaders.h"

namespace Renderer
{
	Material* ConvertAssimpMaterial(const aiScene* assimpScene, const aiMaterial* material)
	{
		aiColor4D diffuseColor;
		aiColor4D specularColor;
		aiColor4D ambientColor;
		float shininess;
		float opacity;

		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
		material->Get(AI_MATKEY_OPACITY, opacity);

		//std::cout << "Specular: " << specularColor.r << ", " << specularColor.g << ", " << specularColor.b << /*", " << cor.a <<*/ std::endl;

		Material* mat = nullptr;
		if (opacity < 1.f)
			mat = new RefractiveMaterial(ConvertAssimpColor(diffuseColor), 1.45f);
		else if (!specularColor.IsBlack())
			mat = new GlossyMaterial(ConvertAssimpColor(diffuseColor));
		else
			mat = new DiffuseMaterial(ConvertAssimpColor(diffuseColor));

		// Load Textures
		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			RGBTexture* diffTexture = LoadTexture(assimpScene, material, aiTextureType_DIFFUSE, 0);
			if (diffTexture)
				mat->SetAlbedoTexture(diffTexture);
		}

		return mat;
	}

	RGBTexture* LoadTexture(const aiScene* assimpScene, const aiMaterial* material, aiTextureType type, uint32_t index)
	{
		aiString path;
		aiTextureMapping mapping;
		uint32_t uv_index;

		aiReturn ret = material->GetTexture(type, index, &path, &mapping, &uv_index);
		if (ret == aiReturn_SUCCESS)
		{
			if (const aiTexture* texture = assimpScene->GetEmbeddedTexture(path.C_Str()))
			{
				//TODO: support embbeded texture
			}
			else
			{
				return RGBTexture::LoadTextureFromFile(path.C_Str());
			}
		}

		return nullptr;
	}

	std::vector<Object*> ConvertAssimpScene(const aiScene* assimpScene, Scene& outRendererScene,
	                                        Material* overrideMaterial)
	{
		if (!assimpScene)
		{
			return {};
		}

		std::vector<Object*> insertedObjects{};
		std::shared_ptr<Material> overrideMat{ overrideMaterial };

		std::list<aiNode*> nodes;
		nodes.push_back(assimpScene->mRootNode);
		int j = 0;
		while (nodes.size())
		{
			aiNode * nd = nodes.back();
			nodes.pop_back();

			for (size_t i = 0; i < nd->mNumMeshes; i++)
			{
				const aiMesh* mesh = assimpScene->mMeshes[nd->mMeshes[i]];

				// Only accepts triangle primitives
				if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE || mesh->mNumVertices == 0)
					continue;

				std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertices;
				vertices.reserve(mesh->mNumVertices);
				std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vNormals;
				std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f>> textCoord;
				std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vTangent;
				std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vBitangent;
				std::vector<Eigen::Vector3i, Eigen::aligned_allocator<Eigen::Vector3i>> indices;
				indices.reserve(mesh->mNumFaces);

				// Populate vertex data
				for (size_t t = 0; t < mesh->mNumVertices; t++)
				{
					const aiVector3D vertex = mesh->mVertices[t];
					vertices.push_back(Eigen::Vector3f(vertex.x, vertex.y, vertex.z));

					if (mesh->HasNormals())
						vNormals.push_back(Eigen::Vector3f(mesh->mNormals[t].x, mesh->mNormals[t].y, mesh->mNormals[t].z));
					else
						std::cout << "mesh has no generated normals!\n";
					
					if (mesh->HasTextureCoords(0))
						textCoord.push_back(Eigen::Vector2f(mesh->mTextureCoords[0][t].x, mesh->mTextureCoords[0][t].y));
					
					if (mesh->mTangents) 
					{
						vTangent.push_back(Eigen::Vector3f(mesh->mTangents[t].x, mesh->mTangents[t].y, mesh->mTangents[t].z));
						vBitangent.push_back(Eigen::Vector3f(mesh->mBitangents[t].x, mesh->mBitangents[t].y, mesh->mBitangents[t].z));
					}
				}

				// Populate faces
				for (size_t t = 0; t < mesh->mNumFaces; ++t) 
				{
					const aiFace* face = &mesh->mFaces[t];
					if (face->mNumIndices == 3)
					{
						indices.push_back(Eigen::Vector3i(face->mIndices[0], face->mIndices[1], face->mIndices[2]));
					}
				}

				Object* scene_mesh = new Mesh(vertices, indices, vNormals, textCoord, vTangent, vBitangent);
				scene_mesh->SetBounds(GetMeshBounds(mesh));
				// Compute material properties
				if (!overrideMat)
				{
					const int materialIndex = mesh->mMaterialIndex;
					const aiMaterial* material = assimpScene->mMaterials[materialIndex];

					scene_mesh->SetMaterial(
						std::shared_ptr<Material>{ConvertAssimpMaterial(assimpScene, material)}
					);
				}
				else
				{
					scene_mesh->SetMaterial(overrideMat);
				}

				std::stringstream ss;
				ss << "mesh[" << i << "]";
				scene_mesh->name = ss.str();
				outRendererScene.InsertObject(scene_mesh);

				insertedObjects.push_back(scene_mesh);
			}

			for (size_t k = 0; k < nd->mNumChildren; k++)
			{
				nodes.push_back(nd->mChildren[k]);
			}
		}

		return std::move(insertedObjects);
	}

}
