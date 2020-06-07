#pragma once
#ifndef MATERIAL_DIFFUSE
#define MATERIAL_DIFFUSE

#include "Shading/Material.h"
namespace Renderer {
	class DiffuseMaterial :
	public Material 
	{
	private:
		

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		DiffuseMaterial(Eigen::Vector3f DiffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f),
			Eigen::Vector3f SpecularColor = Eigen::Vector3f(0.5f, 0.5f, 0.5f),
			Eigen::Vector3f ambientColor = Eigen::Vector3f(0.f, 0.f, 0.f),
			float Reflectivity = 0.5f,
			float Glossiness = 0.0f,
			float Roughness = 0.0f);

		~DiffuseMaterial();

		Eigen::Vector3f getDirectIllumination(Scene &scene, HitInfo& hit_info);



	};
}

#endif