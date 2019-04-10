#pragma once
#include "Object/Object.h"
#include "Shading/Material.h"
namespace Renderer {
	class Triangle :
		public Object
	{
	friend class Mesh;
	private:
		struct TriangleStruct {
			//EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			glm::fvec3* P0, *P1, *P2;
			glm::fvec3* N0, *N1, *N2;
			glm::fvec2* UV0, *UV1, *UV2;
			glm::fvec3* T0, *T1, *T2;
			glm::fvec3* B0, *B1, *B2;
			Renderer::Material* Material;
		};

		glm::fvec3 P0, P1, P2;
		glm::fvec3 u, v, normal;
	public:
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Triangle(glm::fvec3 p0, glm::fvec3 p1, glm::fvec3 p2, glm::fvec3 color = glm::fvec3(0.8f, 0.5f, 0.8f));
		~Triangle();

		bool is_hit_by_ray(Ray* incoming_ray, HitInfo& hit_info);

		static bool triangle_hit_by_ray(TriangleStruct triangle, 
			Ray* incoming_ray,
			HitInfo& hit_info);
	};
}
