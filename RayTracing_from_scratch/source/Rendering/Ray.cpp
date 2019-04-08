#include "pch.h"
#include "Rendering/Ray.h"
using namespace Renderer;

Ray::Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, float Attenuation, int Depth) : origin(Origin), direction(Direction), attenuation(Attenuation), depth(Depth)
{
}

Ray::~Ray()
{
}

Eigen::Vector3f Ray::getOrigin() const
{
	return Eigen::Vector3f(this->origin);
}

Eigen::Vector3f Ray::getDirection() const
{
	return Eigen::Vector3f(this->direction);
}

float Renderer::Ray::getAttenuation() const
{
	return this->attenuation;
}

const int Ray::getDepth() const
{
	return this->depth;
}
