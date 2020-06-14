#include "pch.h"
#include "Rendering/Ray.h"
using namespace Renderer;

Ray::Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, int Depth, bool BackfaceHit, float MediumIndex) : origin(Origin), direction(Direction), 
depth(Depth), isBackfaceHit(BackfaceHit), RefractiveIndex(MediumIndex)
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

const bool Renderer::Ray::getIsBackfaceHit() const
{
	return this->isBackfaceHit;
}

const float Renderer::Ray::getRefractiveIndex() const
{
	return this->RefractiveIndex;
}

const int Ray::getDepth() const
{
	return this->depth;
}
