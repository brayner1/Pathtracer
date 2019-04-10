#pragma once
#include "Rendering/Ray.h"
#include "Shading/Material.h"

#ifndef OBJECT_H
#define OBJECT_H

namespace Renderer {
	//class Material;
	class Object;
	struct HitInfo {
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		glm::fvec3 Point;
		glm::fvec3 Normal;
		glm::fvec3 U_vector, V_vector;
		Object* obj;
		Ray* ray;
		Material * Material;
		//glm::fvec3 Color;
		float U_factor, V_factor, Distance;
		std::vector<Ray> outgoing_rays;
		int x, y, w, h;

		static struct HitInfo resetStruct() {
			HitInfo info;
			info.obj = NULL;
			info.Material = NULL;
			info.Point = info.Normal = glm::fvec3(0.0f, 0.0f, 0.0f);
			info.U_factor = info.V_factor = info.Distance = std::numeric_limits<float>::max();
			info.outgoing_rays.clear();
			return info;
		}
	};

	class Object
	{
	protected:
		Material* Material;
		glm::fvec3 Color;

		//Eigen::AlignedBox3f Object_bounds;
	public:
		std::string name;

		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Object();
		~Object();

		virtual bool is_hit_by_ray(
			Ray* incoming_ray, HitInfo& hit_info) = 0;

		void setColor(glm::fvec3 color);
		void setMaterial(Renderer::Material* material);

	};
}

#endif