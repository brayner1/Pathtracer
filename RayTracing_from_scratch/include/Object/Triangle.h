#pragma once
#include "Object/Object.h"
#include "Shading/Material.h"
namespace Renderer {
	class Material;
	class Triangle :
		public Object
	{
	friend class Mesh;
	private:
		struct TriangleStruct {
			Eigen::Vector3f P0, P1, P2;
			Eigen::Vector3f* N0, *N1, *N2;
			Eigen::Vector2f* UV0, *UV1, *UV2;
			Eigen::Vector3f* T0, *T1, *T2;
			Eigen::Vector3f* B0, *B1, *B2;
			Renderer::Material* Material;
		};

		Eigen::Vector3f P0, P1, P2;
		Eigen::Vector3f u, v, normal;
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Triangle(Eigen::Vector3f p0, Eigen::Vector3f p1, Eigen::Vector3f p2, Eigen::Vector3f color = Eigen::Vector3f(0.8f, 0.5f, 0.8f));
		~Triangle();

		bool is_hit_by_ray(Ray& incoming_ray, HitInfo& hit_info);

		static bool triangle_hit_by_ray(const TriangleStruct& triangle, 
			Ray& incoming_ray,
			HitInfo& hit_info);
	};
}
