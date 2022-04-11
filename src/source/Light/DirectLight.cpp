#include "pch.h"
#include "Light/DirectLight.h"

using namespace Renderer;

Eigen::Vector3f DirectLight::SampleLightIntensity(const Scene& scene, const HitInfo& hit,
	Eigen::Vector3f& lightDirection, float& pdf) const
{
	const Ray lightRay { hit.Point, -direction };
	if (!TestLightVisibility(scene, hit, lightRay))
	{
		pdf = 0.f;
		return Eigen::Vector3f::Zero();
	}
	lightDirection = -direction;
	pdf = 1.f;
	return intensity;
}
