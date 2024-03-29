﻿#ifndef PTracer_PointLight_h
#define PTracer_PointLight_h
  #include "Object/Object.h"
#include "Light/Light.h"

namespace Renderer
{
	class PointLight : public Light
	{
	public:

		explicit PointLight(const Eigen::Vector3f& intensity, const Eigen::Vector3f& position) : Light(intensity)
		{
			this->position = position;
			isDeltaLight = true;
		}

		explicit PointLight(const Eigen::Vector3f& color)
			: Light(color)
		{
		}

		Eigen::Vector3f	SampleLightIntensity(const Scene& scene, const HitInfo& hit, Eigen::Vector3f& lightDirection, float& pdf) const override;
	};

}

#endif