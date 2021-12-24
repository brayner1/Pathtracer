#pragma once
#include "Light.h"

namespace Renderer
{

	class PrimitiveLight : public Light
	{
	public:
		Eigen::Vector3f
		SampleLightIntensity(const HitInfo& hit, Eigen::Vector3f& lightDirection, float& pdf) const override;
		bool TestLightVisibility(const Scene& scene, const HitInfo& hit) const override;
	};

}
