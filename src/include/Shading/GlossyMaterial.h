#pragma once
#ifndef MATERIAL_GLOSSY
#define MATERIAL_GLOSSY
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

		Eigen::Vector3f SampleBSDF(const Eigen::Vector3f& outgoing_ray_dir, const HitInfo& hit_info, Eigen::Vector3f& inbound_ray_dir, float& pdf,
		                           eSampleType& sampled_type) override;
		Eigen::Vector3f BSDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray,
			const HitInfo& hit_info) override;
		float PDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray,
			const HitInfo& hit_info) override;
	};
}

#endif