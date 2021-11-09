#pragma once
#ifndef MATERIAL_GLOSSY
#define MATERIAL_GLOSSY
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>
#include "Shading/Material.h"
namespace Renderer {
	class GlossyMaterial :
	public Material 
	{
	private:
		float roughness;

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		GlossyMaterial(Eigen::Vector3f DiffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f), float Roughness = 0.0f);

		const float getRoughness() const;
		void setRoughness(float Roughness);

		Eigen::Vector3f ObjectHitColor(const Ray&ray, Scene& scene, HitInfo& hit_info) const;
		Eigen::Vector3f getDirectIllumination(const Ray& ray, const Scene& scene, const HitInfo& hit_info) const;
		Eigen::Vector3f SampleBSDF(const Ray& outgoing_ray, const HitInfo& hit_info, Ray& inbound_ray, float& pdf,
			eSampleType& sampled_type) override;
	};
}

#endif