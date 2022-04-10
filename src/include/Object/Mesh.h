#pragma once
#include "Object/Object.h"

namespace Renderer {
	class Mesh final :
		public Object
	{
	public:
		// Mesh Properties
		std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertices;
		std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vNormals;
		std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f>> vUV;
		std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vTangent;
		std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vBitangent;
		std::vector<Eigen::Vector3i, Eigen::aligned_allocator<Eigen::Vector3i>> triangles;

		Eigen::Transform<float, 3, Eigen::TransformTraits::Affine> transf;

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Mesh(std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_array, 
			std::vector<Eigen::Vector3i, Eigen::aligned_allocator<Eigen::Vector3i>> index_array,
			std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_normals_array = std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>(),
			std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f>> texture_coord_array = std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f>>(),
			std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_tangent_array = std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>(),
			std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_bitangent_array = std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>()
		);

		Mesh(Eigen::Vector3f* vertex_array, Eigen::Vector3i* index_array,
			int num_vertices, int num_indices,
			Eigen::Vector3f* vertex_normals_array = nullptr,
			Eigen::Vector2f* texture_coord_array = nullptr,
			Eigen::Vector3f* vertex_tangent_array = nullptr,
			Eigen::Vector3f* vertex_bitangent_array = nullptr
		);

		float PrimitiveHitByRay(const Ray& incoming_ray, int primitive_index, HitInfo& hit_info) const override;
		float PrimitiveHitByRay(const Ray& incoming_ray, int primitive_index) const override;

		[[nodiscard]] std::vector<Eigen::AlignedBox3f> GetPrimitivesBounds() const override;
		uint32_t GetPrimitiveCount() const override;

		HitInfo SamplePrimitivePoint(uint32_t primitiveIndex) const override;
		float PrimitiveSamplePDF(uint32_t primitiveIndex) const override;
	};
}
