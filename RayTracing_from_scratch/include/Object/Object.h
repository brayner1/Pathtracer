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
		Eigen::Vector3f Point;
		Eigen::Vector3f Normal;
		Eigen::Vector2f TextureCoord;
		float U_factor, V_factor, Distance;
		Eigen::Vector3f U_vector, V_vector;
		Eigen::Vector3f Attenuation;
		Eigen::Vector3f Albedo;
		int x, y, w, h;

		static struct HitInfo resetStruct() {
			HitInfo info;
			info.obj = NULL;
			info.Material = NULL;
			info.Point = info.Normal = Eigen::Vector3f::Zero();
			info.TextureCoord = Eigen::Vector2f::Zero();
			info.U_factor = info.V_factor = info.Distance = 0.0f;
			info.Attenuation = Eigen::Vector3f::Ones();
			info.Albedo = Eigen::Vector3f::Zero();
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