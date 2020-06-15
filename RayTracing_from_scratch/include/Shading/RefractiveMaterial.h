#pragma once
#ifndef MATERIAL_REFRACTIVE
#define MATERIAL_REFRACTIVE

#include "Shading/Material.h"
namespace Renderer {
	class RefractiveMaterial :
	public Material 
	{
	private:
		
		float RefractiveIndex;

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		RefractiveMaterial(Eigen::Vector3f DiffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f), float RefractiveIndex = 1.5f);

		~RefractiveMaterial();

		Eigen::Vector3f ObjectHitColor(Scene& scene, HitInfo& hit_info);
		Eigen::Vector3f getDirectIllumination(Scene &scene, HitInfo& hit_info);



	};
}

#endif