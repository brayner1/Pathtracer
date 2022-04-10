#pragma once
#include "Shading/Material.h"

namespace Renderer
{
	

	class PhongMaterial : public Material
	{
	public:
		PhongMaterial(const Eigen::Vector3f& color, float kd, float ks, float kt, float n, float ior);

		Eigen::Vector3f SampleBSDF(const Eigen::Vector3f& outgoing_ray_dir, const HitInfo& hit_info, Eigen::Vector3f& inbound_ray_dir, float& pdf, eSampleType& sampled_type) override;

		float GetIor() const override { return ior; }
		Eigen::Vector3f BSDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray,
			const HitInfo& hit_info) override;
		float PDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray,
			const HitInfo& hit_info) override;

	protected:
		float kd, ks, kt, n, ior;
	};

}