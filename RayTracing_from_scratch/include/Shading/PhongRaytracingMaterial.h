#pragma once
#ifndef MATERIAL_PHONG_H
#define MATERIAL_PHONG_H

#include "Shading/Material.h"
namespace Renderer {
	class PhongRaytracingMaterial :
	public Material 
	{
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		PhongRaytracingMaterial(Eigen::Vector3f DiffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f),
			Eigen::Vector3f SpecularColor = Eigen::Vector3f(0.5f, 0.5f, 0.5f),
			Eigen::Vector3f ambientColor = Eigen::Vector3f(0.f, 0.f, 0.f),
			float Reflectivity = 0.5f,
			float Glossiness = 0.0f,
			float Roughness = 0.0f);

		~PhongRaytracingMaterial();

		Eigen::Vector3f getDirectIllumination(Scene &scene, HitInfo& hit_info);

	};
}

#endif