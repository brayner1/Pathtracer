#include "pch.h"
#include "Shading/DiffuseMaterial.h"
#include "RenderHeaders.h"
using namespace Renderer;


DiffuseMaterial::DiffuseMaterial(const Eigen::Vector3f& DiffuseCcolor) : Material(DiffuseCcolor)
{
}

Eigen::Vector3f DiffuseMaterial::SampleBSDF(const Eigen::Vector3f& outgoing_ray_dir, const HitInfo& hit_info, Eigen::Vector3f& inbound_ray_dir,
                                            float& pdf, eSampleType& sampled_type)
{
	sampled_type = static_cast<eSampleType>(BSDF_DIFFUSE | BSDF_REFLECTION);
	inbound_ray_dir = sample_hemisphere_vector_cosine(hit_info.surfNormal, hit_info.U_vector);
	assert(inbound_ray_dir.norm() - 1.f < std::numeric_limits<float>::epsilon() && "sampled ray direction must be normalized.");
	pdf = std::abs(inbound_ray_dir.dot(hit_info.surfNormal)) * M_1_PI;

	return this->GetAlbedo(hit_info.UvCoord) * M_1_PI;
}

Eigen::Vector3f DiffuseMaterial::BSDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray,
	const HitInfo& hit_info)
{
	return this->GetAlbedo(hit_info.UvCoord) * M_1_PI;
}

float DiffuseMaterial::PDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray,
	const HitInfo& hit_info)
{
	return (outoing_ray.dot(inbound_ray) > 0) ? abs(inbound_ray.dot(hit_info.surfNormal)) * M_1_PI : 0.f;
}
