#include "pch.h"
#include "Rendering/Ray.h"
using namespace Renderer;

Ray::Ray(Eigen::Vector4f& Origin, Eigen::Vector4f& Direction, int Depth, bool BackfaceHit, float MediumIndex) : Origin(Origin), Direction(Direction), 
depth(Depth), isBackfaceHit(BackfaceHit), RefractiveIndex(MediumIndex)
{}

Renderer::Ray::Ray(Eigen::Vector4f& Origin, Eigen::Vector4f& Direction, float MaxDistance) : Origin(Origin), Direction(Direction), MaxDistance(MaxDistance)
{}

Ray::~Ray()
{
}

const Eigen::Vector4f& Ray::getOrigin() const
{
	return this->Origin;
}

const Eigen::Vector4f& Ray::getDirection() const
{
	return this->Direction;
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
