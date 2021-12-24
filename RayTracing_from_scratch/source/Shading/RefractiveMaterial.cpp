#include "pch.h"
#include "Shading/RefractiveMaterial.h"
#include "RenderHeaders.h"
using namespace Renderer;


RefractiveMaterial::RefractiveMaterial(Eigen::Vector3f DiffuseCcolor, float RefractiveIndex) : Material(DiffuseCcolor), refractiveIndex(RefractiveIndex)
{
	
}

Eigen::Vector3f RefractiveMaterial::SampleBSDF(const Eigen::Vector3f& outgoing_ray_dir, const HitInfo& hit_info, Eigen::Vector3f& inbound_ray_dir,
                                               float& pdf, eSampleType& sampled_type)
{
	float cosI = outgoing_ray_dir.dot(hit_info.surfNormal);
	const bool entering = cosI > 0.f;
	cosI = std::abs(cosI);
	const float etaI = entering? 1.f : refractiveIndex;
	const float etaT = entering? refractiveIndex : 1.f;
	const float sinT = (etaI/etaT) * std::sqrt(std::max(0.f, 1.0f - cosI * cosI));
	float fr = 1.f;
	if (sinT < 1.f)
	{
		const float cosT = std::sqrt(1.0f - sinT * sinT);
		fr = FrDieletric(cosI, cosT, etaI, etaT, hit_info.x, hit_info.y);
		const float sample = uniform_random_01();
		if (sample >= fr)
		{
			inbound_ray_dir = Refract(outgoing_ray_dir, FaceForward(outgoing_ray_dir, hit_info.surfNormal), cosI, cosT, etaI/etaT);
			pdf = 1.f - fr;
			sampled_type = static_cast<eSampleType>(BSDF_SPECULAR | BSDF_TRANSMISSION);

			return diffuse_color * (1.f - fr) * ((etaI*etaI)/(etaT*etaT)) / std::abs(inbound_ray_dir.dot(hit_info.surfNormal));
		}
	}
	
	inbound_ray_dir = Reflect(outgoing_ray_dir, hit_info.surfNormal);
	pdf = fr;
	sampled_type = static_cast<eSampleType>(BSDF_SPECULAR | BSDF_REFLECTION);

	const float cosr = 1.f / std::abs(inbound_ray_dir.dot(hit_info.surfNormal));
	return diffuse_color * fr * cosr;
}
