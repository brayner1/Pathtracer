#include "pch.h"
#include "Rendering/Light.h"
using namespace Renderer;

Light::Light(glm::fvec3& Color) : color(Color)
{
}


Light::~Light()
{
}

void Renderer::Light::setColor(float red, float green, float blue)
{
	this->color = glm::fvec3(red, green, blue);
}

void Renderer::Light::setColor(glm::fvec3& Color)
{
	color = Color;
}

glm::fvec3 Renderer::Light::getColor() const
{
	return this->color;
}

void Renderer::Light::setPosition(const glm::fvec3& position)
{
	//this->transform.translate(position);
	this->position = position;
}

glm::fvec3 Renderer::Light::getPosition() const
{
	return this->position;
}

glm::fmat3 Renderer::Light::getRotation() const
{
	return glm::fmat3(this->transform);//this->transform.rotation();
}

glm::fvec3 Renderer::Light::getDirection() const
{
	return glm::fmat3(this->transform) * glm::fvec3(0.0f, 0.0f, -1.0f);//-this->transform.rotation().col(1);
}
