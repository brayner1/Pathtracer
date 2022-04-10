#include "pch.h"
#include "Light/PointLight.h"
#include "Rendering/Scene.h"

Eigen::Vector3f Renderer::PointLight::SampleLightIntensity(const Scene& scene, const HitInfo& hit,
                                                           Eigen::Vector3f& lightDirection, float& pdf) const
{
	
	Eigen::Vector3f diff = position - hit.Point;
	const float distance_sqr = diff.dot(diff);
	diff.normalize();
	{
		const Ray lightRay {hit.Point, diff, std::sqrt(distance_sqr)};
		if (!TestLightVisibility(scene, hit, lightRay))
		{
			pdf = 0.f;
			return Eigen::Vector3f::Zero();
		}
	}
	lightDirection = diff;
	pdf = 1.f;
	return (intensity / distance_sqr);
}