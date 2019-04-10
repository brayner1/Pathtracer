#include "pch.h"
#include "Rendering/Ray.h"
using namespace Renderer;

Ray::Ray(glm::fvec3& Origin, glm::fvec3& Direction, float Attenuation, int Depth) : origin(Origin), direction(Direction), attenuation(Attenuation), depth(Depth)
{
}

Ray::~Ray()
{
}

glm::fvec3 Ray::getOrigin() const
{
	return this->origin;
}

glm::fvec3 Ray::getDirection() const
{
	return this->direction;
}

float Renderer::Ray::getAttenuation() const
{
	return this->attenuation;
}

const int Ray::getDepth() const
{
	return this->depth;
}
