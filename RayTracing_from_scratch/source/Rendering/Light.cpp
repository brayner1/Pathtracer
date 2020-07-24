#include "pch.h"
#include "Rendering/Light.h"
using namespace Renderer;

Light::Light(Eigen::Vector3f Color) : color(Color)
{
}


Light::~Light()
{
}

void Renderer::Light::setColor(float red, float green, float blue)
{
	this->color = Eigen::Vector3f(red, green, blue);
}

void Renderer::Light::setColor(Eigen::Vector3f Color)
{
	color = Color;
}

Eigen::Vector4f Renderer::Light::getColor() const
{
	Eigen::Vector3f c = this->color;
	return Eigen::Vector4f(c.x(), c.y(), c.z(), 0.0f);
}

void Renderer::Light::setPosition(const Eigen::Vector3f position)
{
	this->transform.translate(position);
	this->position = position;
}

Eigen::Vector4f Renderer::Light::getPosition() const
{
	Eigen::Vector3f p = this->position;
	return Eigen::Vector4f(p.x(), p.y(), p.z(), 1.0f);
}

Eigen::Matrix3f Renderer::Light::getRotation() const
{
	return this->transform.rotation();
}

Eigen::Vector3f Renderer::Light::getDirection() const
{
	return -this->transform.rotation().col(1);
}
