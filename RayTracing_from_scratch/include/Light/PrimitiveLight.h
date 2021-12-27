#pragma once
#include "Light.h"

namespace Renderer
{

	class PrimitiveLight : public Light
	{
	public:
		PrimitiveLight(const Eigen::Vector3f& intensity, Object* object, uint32_t primitive_index) : Light(intensity), object(object), primitiveIdx(primitive_index) {}

		Eigen::Vector3f SampleLightIntensity(const ::Renderer::Scene& scene, const HitInfo& hit, Eigen::Vector3f& lightDirection, float& pdf) const override;

	private:
		Object* object = nullptr;
		uint32_t primitiveIdx = 0;
	};

}
