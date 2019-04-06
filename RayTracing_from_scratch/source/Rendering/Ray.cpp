#include "pch.h"
#include "Rendering/Ray.h"
using namespace Renderer;

Ray::Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, int Depth) : origin(Origin), direction(Direction), depth(Depth)
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

const int Ray::getDepth() const
{
	return this->depth;
}
