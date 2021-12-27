#include "pch.h"
#include "Object/Object.h"
#include "Shading/PhongMaterial.h"
#include "RandUtil.h"

namespace Renderer
{
	PhongMaterial::PhongMaterial(const Eigen::Vector3f& color, float kd, float ks, float kt, float n, float ior) :
	Material(color), kd(kd), ks(ks), kt(kt), n(n), ior(ior) { }

	Eigen::Vector3f PhongMaterial::SampleBSDF(const Eigen::Vector3f& outgoing_ray_dir, const HitInfo& hit_info,
		Eigen::Vector3f& inbound_ray_dir, float& pdf, eSampleType& sampled_type)
	{
		const float ktot = kd + ks + kt;
		const float choice = uniform_random_01() * ktot; // random number between [0,ktot[

		if (choice < kd)
		{
			// choice chose to sample the diffuse part
			sampled_type = static_cast<eSampleType>(BSDF_DIFFUSE | BSDF_REFLECTION);
			inbound_ray_dir = sample_hemisphere_vector_cosine(hit_info.surfNormal, hit_info.U_vector);
			pdf = (std::abs(inbound_ray_dir.dot(hit_info.surfNormal)) * M_1_PI) * kd / ktot;

			return this->GetAlbedo(hit_info.UvCoord) * kd / ktot * M_1_PI;
		}
		else if (choice < kd + ks)
		{
			// choice chose the specular reflection part
			sampled_type = static_cast<eSampleType>(BSDF_SPECULAR | BSDF_REFLECTION);
			inbound_ray_dir = Reflect(outgoing_ray_dir, hit_info.surfNormal);
			pdf = ks / ktot;

			const float cosr = 1.f / std::abs(inbound_ray_dir.dot(hit_info.surfNormal));
			return this->GetAlbedo(hit_info.UvCoord) * ks / ktot * cosr;
		}
		else
		{
			// TODO: choice chose the specular transmission part
			float cosI = outgoing_ray_dir.dot(hit_info.surfNormal);
			const bool entering = cosI > 0.f;
			cosI = std::abs(cosI);
			const float etaI = entering? 1.f : ior;
			const float etaT = entering? ior : 1.f;
			const float sinT = (etaI/etaT) * std::sqrt(std::max(0.f, 1.0f - cosI * cosI));
			if (sinT < 1.f)
			{
				const float cosT = std::sqrt(1.0f - sinT * sinT);

				inbound_ray_dir = Refract(outgoing_ray_dir, FaceForward(outgoing_ray_dir, hit_info.surfNormal), cosI, cosT, etaI/etaT);
				pdf = kt / ktot;
				sampled_type = static_cast<eSampleType>(BSDF_SPECULAR | BSDF_TRANSMISSION);

				return this->GetAlbedo(hit_info.UvCoord) * kt / ktot * ((etaI*etaI)/(etaT*etaT)) / std::abs(inbound_ray_dir.dot(hit_info.surfNormal));
			}
			// choice chose the total internal reflection part
			sampled_type = static_cast<eSampleType>(BSDF_SPECULAR | BSDF_REFLECTION);
			inbound_ray_dir = Reflect(outgoing_ray_dir, hit_info.surfNormal);
			pdf = kt / ktot;

			const float cosr = 1.f / std::abs(inbound_ray_dir.dot(hit_info.surfNormal));
			return this->GetAlbedo(hit_info.UvCoord) * kt / ktot * cosr;
		}
	}
}
