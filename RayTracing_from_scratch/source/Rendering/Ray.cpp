#include "pch.h"
#include "Rendering/Ray.h"
using namespace Renderer;

Ray::Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, int Depth, bool BackfaceHit, float MediumIndex) : Origin(Origin), Direction(Direction), 
depth(Depth), isBackfaceHit(BackfaceHit), RefractiveIndex(MediumIndex)
{}

Renderer::Ray::Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, float MaxDistance) : Origin(Origin), Direction(Direction), MaxDistance(MaxDistance)
{}

Ray::~Ray()
{
}

Eigen::Vector3f Ray::getOrigin() const
{
	return Eigen::Vector3f(this->Origin);
}

Eigen::Vector3f Ray::getDirection() const
{
	return Eigen::Vector3f(this->Direction);
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
