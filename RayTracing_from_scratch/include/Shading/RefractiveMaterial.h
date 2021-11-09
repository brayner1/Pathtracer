#pragma once
#ifndef MATERIAL_REFRACTIVE
#define MATERIAL_REFRACTIVE
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
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

		Eigen::Vector3f ObjectHitColor(const Ray&ray, Scene& scene, HitInfo& hit_info) const;
		Eigen::Vector3f getDirectIllumination(const Ray& ray, const Scene& scene, const HitInfo& hit_info) const;
		Eigen::Vector3f SampleBSDF(const Ray& outgoing_ray, const HitInfo& hit_info, Ray& inbound_ray, float& pdf,
			eSampleType& sampled_type) override;
	};
}

#endif