#include "pch.h"
#include "Shading/BxDF.h"
#include "RenderHeaders.h"

using namespace Renderer;

//Renderer::Spectrum Renderer::BxDF::sample_f(HitInfo& hit, Eigen::Vector3f& LightRay, const Eigen::Vector2f* Sample, float* pdf, BxDFType* sampledType /*= nullptr*/) const
//{
//	LightRay = random_hemisphere_vector(hit.Normal, hit.U_vector);
//
//	*pdf = Pdf(hit, LightRay);
//
//	return f(hit, LightRay);
//}
//
//Renderer::Spectrum Renderer::BxDF::reflectance(HitInfo& hit, int nSamples, const Eigen::Vector2f& samples) const
//{
//	//Spectrum r = 0.0f;
//	//for (int i  = 0; i < nSamples; i++)
//	//{
//	//	Eigen::Vector3f InDir = Eigen::Vector3f::Zero();
//	//	float pdf = 0.0f;
//	//	//Spectrum f = sample_f(hit, InDir, Eigen::Vector2f(0.0f, 0.0f), &pdf);
//	//}
//}
//
//float Renderer::BxDF::Pdf(const HitInfo& hit, const Eigen::Vector3f& InRay) const
//{
//	return hit.Normal.dot(InRay) / M_PI;
//}
