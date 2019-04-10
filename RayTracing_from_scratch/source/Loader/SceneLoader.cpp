#include "pch.h"
#include "Loader/SceneLoader.h"
#include "Renderer.h"
//#include "Object/Mesh.h"
using namespace Renderer;

//Eigen::AlignedBox3f SceneLoader::get_bounding_box_for_node(const aiNode * nd, aiMatrix4x4t<float> * trafo, Eigen::AlignedBox3f previous_boundingBox)
//{
//	aiMatrix4x4t<float> prev = aiMatrix4x4t<float>(*trafo);
//	unsigned int n = 0, t;
//
//	Eigen::Vector3f min, max;
//	*trafo *= (nd->mTransformation);
//	//aiMultiplyMatrix4(trafo, &nd->mTransformation);
//
//	for (; n < nd->mNumMeshes; ++n) {
//		const struct aiMesh* mesh = this->assimp_scene->mMeshes[nd->mMeshes[n]];
//		for (t = 0; t < mesh->mNumVertices; ++t) {
//
//			aiVector3D tmp = mesh->mVertices[t];
//			tmp *= *trafo;
//			//aiTransformVecByMatrix4(&tmp, trafo);
//			min(0) = (min.x() < tmp.x)? min.x() : tmp.x;
//			min(1) = (min.y() < tmp.y)? min.y() : tmp.y;
//			min(2) = (min.z() < tmp.z)? min.z() : tmp.z;
//
//			max(0) = (max.x() > tmp.x) ? max.x() : tmp.x;
//			max(1) = (max.y() > tmp.y) ? max.y() : tmp.y;
//			max(2) = (max.z() > tmp.z) ? max.z() : tmp.z;
//		}
//	}
//	Eigen::AlignedBox3f boundingBox = previous_boundingBox.merged(Eigen::AlignedBox3f(min, max));
//	for (n = 0; n < nd->mNumChildren; ++n) {
//		boundingBox = get_bounding_box_for_node(nd->mChildren[n], trafo, boundingBox);
//	}
//	*trafo = prev;
//	return boundingBox;
//}

SceneLoader::SceneLoader(std::string file_path)
{
	this->assimp_scene = this->importer.ReadFile(file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (!this->assimp_scene) {
		std::cout << "Error on load:" << std::endl << this->importer.GetErrorString() << std::endl;
	}
}

void SceneLoader::convertAssimpScene()
{	
	//std::
	std::vector<aiNode*> nodes;
	nodes.push_back(this->assimp_scene->mRootNode);
	while (nodes.size()) {
		aiNode * nd = nodes.back();
		nodes.pop_back();
		for (size_t i = 0; i < nd->mNumMeshes; i++)
		{
			const struct aiMesh* mesh = this->assimp_scene->mMeshes[nd->mMeshes[i]];
			
			std::vector<glm::fvec3> vertices;
			std::vector<glm::fvec3> vNormals;
			std::vector<glm::fvec3> vTangent;
			std::vector<glm::fvec3> vBitangent;
			std::vector<glm::fvec2> textCoord;
			std::vector<glm::ivec3> indices;
			// Populate vertices
			for (size_t t = 0; t < mesh->mNumVertices; t++)
			{
				const aiVector3D vertex = mesh->mVertices[t];
				vertices.push_back(glm::fvec3(vertex.x, vertex.y, vertex.z));
				if (mesh->HasNormals()) {
					//std::cout << "Loader: mesh has normals" << std::endl;
					vNormals.push_back(glm::fvec3(mesh->mNormals[t].x, mesh->mNormals[t].y, mesh->mNormals[t].z));
				}
				if (mesh->HasTextureCoords(0))
					textCoord.push_back(glm::fvec2(mesh->mTextureCoords[0][t].x, mesh->mTextureCoords[0][t].y));
				if (mesh->HasTangentsAndBitangents()) {
					vTangent.push_back(glm::fvec3(mesh->mTangents[t].x, mesh->mTangents[t].y, mesh->mTangents[t].z));
					vBitangent.push_back(glm::fvec3(mesh->mBitangents[t].x, mesh->mBitangents[t].y, mesh->mBitangents[t].z));
				}
			}

			// Populate indices
			for (size_t t = 0; t < mesh->mNumFaces; ++t) {
				const aiFace* face = &mesh->mFaces[t];
				if (face->mNumIndices == 3) {
					indices.push_back(glm::ivec3(face->mIndices[0], face->mIndices[1], face->mIndices[2]));
				}
			}
			std::cout << "vertices: " << vertices.size() << std::endl;
			std::cout << "normals: " << vNormals.size() << std::endl;
			int materialIndex = mesh->mMaterialIndex;
			aiMaterial* material = this->assimp_scene->mMaterials[materialIndex];
			//aiPropertyTypeInfo type = prop->mType;
			aiColor4D specularColor;
			aiColor4D diffuseColor;
			aiColor4D ambientColor;
			float shininess;

			aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);
			aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specularColor);
			aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambientColor);
			aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);
			//mat->Get(key.C_Str(), 0, 0, cor);
			
			std::cout << "Data: " << specularColor.r << ", " << specularColor.g << ", " << specularColor.b << /*", " << cor.a <<*/ std::endl;

			Object* scene_mesh = new Mesh(vertices, indices, vNormals, textCoord, vTangent, vBitangent);
			PhongDiffuseMaterial* converted_material = new PhongDiffuseMaterial(
				*convert_assimp_color(diffuseColor),
				*convert_assimp_color(specularColor),
				*convert_assimp_color(ambientColor),
				0.2f,
				shininess,
				0.032f);
			scene_mesh->setMaterial(converted_material);
			std::stringstream ss;
			ss << "mesh[" << i << "]";
			scene_mesh->name = ss.str();
			this->renderer_scene.insertObject(scene_mesh);
		}
		for (size_t k = 0; k < nd->mNumChildren; k++)
		{
			nodes.push_back(nd->mChildren[k]);
		}
	}
}

const Scene SceneLoader::getRendererScene()
{
	return this->renderer_scene;
}

SceneLoader::~SceneLoader()
{
}
