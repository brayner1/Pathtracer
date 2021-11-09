#ifndef MATERIAL_DIFFUSE
#define MATERIAL_DIFFUSE
#include "Shading/Material.h"
namespace Renderer {
	class DiffuseMaterial :
	public Material 
	{
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		DiffuseMaterial(Eigen::Vector3f DiffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f));

		Eigen::Vector3f ObjectHitColor(const Ray&ray, Scene& scene, HitInfo& hit_info) const override;
		Eigen::Vector3f getDirectIllumination(const Ray& ray, const Scene& scene, const HitInfo& hit_info) const override;
		Eigen::Vector3f SampleBSDF(const Ray& outgoing_ray, const HitInfo& hit_info, Ray& inbound_ray,
		                           float& pdf, eSampleType& sampled_type) override;
	};
}

#endif