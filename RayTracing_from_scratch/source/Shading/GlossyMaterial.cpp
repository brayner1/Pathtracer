#include "pch.h"
#include "Shading/GlossyMaterial.h"
#include "RenderHeaders.h"
using namespace Renderer;


GlossyMaterial::GlossyMaterial(Eigen::Vector3f DiffuseCcolor, float Roughness) : Material(DiffuseCcolor), roughness(Roughness)
{
	
}


//GlossyMaterial::~GlossyMaterial()
//{
//}

const float Renderer::GlossyMaterial::getRoughness() const
{
	return this->roughness;
}

void Renderer::GlossyMaterial::setRoughness(float Roughness)
{
	this->roughness = Roughness;
}

Eigen::Vector3f GlossyMaterial::SampleBSDF(const Eigen::Vector3f& outgoing_ray_dir, const HitInfo& hit_info, Eigen::Vector3f& inbound_ray_dir,
                                           float& pdf, eSampleType& sampled_type)
{
	sampled_type = static_cast<eSampleType>(BSDF_SPECULAR | BSDF_REFLECTION);
	inbound_ray_dir = Reflect(outgoing_ray_dir, hit_info.surfNormal);
	pdf = 1.f;

	const float cosr = 1.f / std::abs(inbound_ray_dir.dot(hit_info.surfNormal));
	return diffuse_color * cosr;
}

Eigen::Vector3f GlossyMaterial::BSDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray,
	const HitInfo& hit_info)
{
	return Eigen::Vector3f::Zero();
}

float GlossyMaterial::PDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray,
	const HitInfo& hit_info)
{
	return 0.f;
}
