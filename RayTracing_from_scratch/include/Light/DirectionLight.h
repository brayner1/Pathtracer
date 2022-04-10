#pragma once
#include "Light.h"
namespace Renderer
{
	class DirectionLight :
	public Light
	{
	public:
		explicit DirectionLight(const Eigen::Vector3f& color, const Eigen::Vector3f& direction)
			: Light(color), direction(direction)
		{
			isDeltaLight = true;
		}

		Eigen::Vector3f SampleLightIntensity(const Scene& scene, const HitInfo& hit, Eigen::Vector3f& lightDirection,
		                                     float& pdf) const override;

	private:
		Eigen::Vector3f direction;
	};
}

