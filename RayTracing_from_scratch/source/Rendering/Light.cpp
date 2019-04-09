#include "pch.h"
#include "Rendering/Light.h"
using namespace Renderer;

Light::Light(Eigen::Vector3f& Color) : color(Color)
{
}


Light::~Light()
{
}

void Renderer::Light::setColor(float red, float green, float blue)
{
	this->color = Eigen::Vector3f(red, green, blue);
}

void Renderer::Light::setColor(Eigen::Vector3f& Color)
{
	color = Color;
}

Eigen::Vector3f Renderer::Light::getColor() const
{
	return this->color;
}

void Renderer::Light::setPosition(const Eigen::Vector3f& position)
{
	this->transform.translate(position);
	this->position = position;
}

Eigen::Vector3f Renderer::Light::getPosition() const
{
	return this->position;
}

Eigen::Matrix3f Renderer::Light::getRotation() const
{
	return this->transform.rotation();
}

Eigen::Vector3f Renderer::Light::getDirection() const
{
	return -this->transform.rotation().col(1);
}
