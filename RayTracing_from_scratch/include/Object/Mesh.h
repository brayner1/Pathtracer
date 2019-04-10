#pragma once
#include "Object/Object.h"
namespace Renderer {
	class Mesh :
		public Object
	{
	private:
		std::vector<glm::fvec3> vertices;
		std::vector<glm::fvec3> vNormals;
		std::vector<glm::fvec3> vTangent;
		std::vector<glm::fvec3> vBitangent;
		std::vector<glm::fvec2> textCoord;
		std::vector<glm::ivec3> indices;

		//Eigen::Transform<float, 3, Eigen::TransformTraits::Affine> transf;
		glm::fmat4 transform;
	public:
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Mesh(std::vector<glm::fvec3> vertex_array, 
			std::vector<glm::ivec3> index_array,
			std::vector<glm::fvec3> vertex_normals_array = std::vector<glm::fvec3>(),
			std::vector<glm::fvec2> texture_coord_array = std::vector<glm::fvec2>(),
			std::vector<glm::fvec3> vertex_tangent_array = std::vector<glm::fvec3>(),
			std::vector<glm::fvec3> vertex_bitangent_array = std::vector<glm::fvec3>()
		);

		Mesh(glm::fvec3* vertex_array, glm::ivec3* index_array,
			int num_vertices, int num_indices,
			glm::fvec3* vertex_normals_array = nullptr,
			glm::fvec2* texture_coord_array = nullptr,
			glm::fvec3* vertex_tangent_array = nullptr,
			glm::fvec3* vertex_bitangent_array = nullptr
		);

		bool is_hit_by_ray(Ray* incoming_ray, HitInfo& hit_info);

		~Mesh();
	};
}
