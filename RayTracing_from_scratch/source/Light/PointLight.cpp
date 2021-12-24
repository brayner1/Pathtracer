#include "pch.h"
#include "Light/PointLight.h"
#include "Rendering/Scene.h"

Eigen::Vector3f Renderer::PointLight::SampleLightIntensity(const HitInfo& hit, Eigen::Vector3f& lightDirection,
                                                           float& pdf) const
{
	Eigen::Vector3f diff = position - hit.Point;
	const float distance_sqr = diff.dot(diff);
	diff.normalize();
	lightDirection = diff;
	pdf = 1.f;
	return (intensity / distance_sqr);
}

bool Renderer::PointLight::TestLightVisibility(const Scene& scene, const HitInfo& hit) const
{
	Eigen::Vector3f diff = position - hit.Point;
	const float distance = diff.norm();
	diff.normalize();
	const Ray lightRay {hit.Point, diff};
	const float t = scene.RayCast(lightRay);
	return t <= 0.f || t >= distance;
}