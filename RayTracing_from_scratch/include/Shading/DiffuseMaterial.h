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

		DiffuseMaterial(Eigen::Vector3f DiffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f));

		~DiffuseMaterial();

		Eigen::Vector3f ObjectHitColor(Scene& scene, HitInfo& hit_info, int nSamples);
		Eigen::Vector3f getDirectIllumination(Scene &scene, HitInfo& hit_info);



	};
}

#endif