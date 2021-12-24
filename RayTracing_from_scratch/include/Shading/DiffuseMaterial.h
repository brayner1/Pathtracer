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

		Eigen::Vector3f SampleBSDF(const Eigen::Vector3f& outgoing_ray_dir, const HitInfo& hit_info, Eigen::Vector3f& inbound_ray_dir,
		                           float& pdf, eSampleType& sampled_type) override;
	};
}

#endif