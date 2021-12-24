#include "pch.h"
#include "Light/Light.h"

namespace Renderer
{
	Light::Light(const Eigen::Vector3f& Color) : intensity(Color)
	{
	}


	Light::~Light()
	{
	}

	void Light::SetColor(const Eigen::Vector3f& Color)
	{
		intensity = Color;
	}

	Eigen::Vector3f Light::getColor() const
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