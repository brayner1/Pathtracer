#include "pch.h"
#include "Rendering/Ray.h"
using namespace Renderer;

Ray::Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, int Depth, float MediumIndex) : Origin(Origin), Direction(Direction), 
depth(Depth), RefractiveIndex(MediumIndex)
{}

Renderer::Ray::Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, float MaxDistance) : Origin(Origin), Direction(Direction), MaxDistance(MaxDistance)
{}
