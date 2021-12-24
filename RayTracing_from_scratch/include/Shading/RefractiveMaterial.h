#pragma once
#ifndef MATERIAL_REFRACTIVE
#define MATERIAL_REFRACTIVE
#include "Shading/Material.h"
namespace Renderer {
	class RefractiveMaterial :
	public Material 
	{
	private:
		
		float refractiveIndex;

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		RefractiveMaterial(Eigen::Vector3f DiffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f), float RefractiveIndex = 1.5f);

		Eigen::Vector3f SampleBSDF(const Eigen::Vector3f& outgoing_ray_dir, const HitInfo& hit_info, Eigen::Vector3f& inbound_ray_dir, float& pdf,
		                           eSampleType& sampled_type) override;

		float getIOR() { return refractiveIndex; }
	};
}

#endif