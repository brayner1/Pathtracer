#include "pch.h"
#include "Light/Light.h"
#include "Rendering/Scene.h"

namespace Renderer
{
	Light::Light(const Eigen::Vector3f& Color) : intensity(Color)
	{
	}

	bool Light::TestLightVisibility(const Scene& scene, const HitInfo& surfHit, const Ray& lightRay) const
	{
		const float t = scene.RayCast(lightRay);
		return t <= 0.f;
	}

	void Light::SetColor(const Eigen::Vector3f& Color)
	{
		intensity = Color;
	}

	Eigen::Vector3f Light::GetColor() const
	{
		return this->intensity;
	}

	void Light::SetPosition(const Eigen::Vector3f& position)
	{
		this->transform.translate(position);
		this->position = position;
	}

	Eigen::Vector3f Light::getPosition() const
	{
		return this->position;
	}

	Eigen::Matrix3f Light::getRotation() const
	{
		return this->transform.rotation();
	}

	Eigen::Vector3f Light::getDirection() const
	{
		return -this->transform.rotation().col(1);
	}

}