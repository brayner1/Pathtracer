#ifndef Ptracer_RefractiveMaterial_h
#define Ptracer_RefractiveMaterial_h
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

		float GetIor() const override { return refractiveIndex; }
		Eigen::Vector3f BSDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray,
			const HitInfo& hit_info) override;
		float PDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray,
			const HitInfo& hit_info) override;
	};
}

#endif