#pragma once
#include "Rendering/Ray.h"
#include "Shading/Material.h"

#ifndef OBJECT_H
#define OBJECT_H

namespace Renderer {
	//class Material;
	class Object;
	struct HitInfo {
		Object* obj;
		Ray* ray;
		Material * Material;
		//Eigen::Vector3f Color;
		Eigen::Vector3f Point;
		Eigen::Vector3f Normal;
		float U_factor, V_factor, Distance;
		Eigen::Vector3f U_vector, V_vector;
		std::vector<Ray> outgoing_rays;
		int x, y, w, h;

		static struct HitInfo resetStruct() {
			HitInfo info;
			info.obj = NULL;
			info.Material = NULL;
			//info.Color = info.Point = info.Normal = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
			info.U_factor = info.V_factor = info.Distance = 0.0f;
			info.outgoing_rays.clear();
			return info;
		}
	};

	class Object
	{
	protected:
		Material* Material;
		Eigen::Vector3f Color;

		Eigen::AlignedBox3f Object_bounds;
	public:
		std::string name;

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Object();
		~Object();

		virtual bool is_hit_by_ray(
			Ray incoming_ray, HitInfo& hit_info) = 0;

		void setColor(Eigen::Vector3f color);
		void setMaterial(Renderer::Material* material);

	};
}

#endif