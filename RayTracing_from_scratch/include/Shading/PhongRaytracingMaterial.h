#pragma once
#ifndef MATERIAL_PHONG_H
#define MATERIAL_PHONG_H

#include "Shading/Material.h"
namespace Renderer {
	class PhongDiffuseMaterial :
	public Material 
	{
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		PhongDiffuseMaterial(Eigen::Vector3f& DiffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f),
			Eigen::Vector3f& SpecularColor = Eigen::Vector3f(0.5f, 0.5f, 0.5f),
			Eigen::Vector3f& ambientColor = Eigen::Vector3f(0.f, 0.f, 0.f),
			float Reflectivity = 0.5f,
			float Glossiness = 0.0f,
			float Roughness = 0.0f);

		~PhongDiffuseMaterial();

		Eigen::Vector3f get_direct_illumination(Scene* scene, HitInfo& hit_info);
		Eigen::Vector3f get_indirect_illumination(Scene* scene, HitInfo& hit_info);

		//Eigen::Vector4f get_hit_color(Scene scene, HitInfo& hit_info, int x, int y);

	};
}

#endif