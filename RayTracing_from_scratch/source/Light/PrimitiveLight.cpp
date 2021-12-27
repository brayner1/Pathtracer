#include "pch.h"
#include "Light/PrimitiveLight.h"

namespace Renderer
{
	Eigen::Vector3f PrimitiveLight::SampleLightIntensity(const Scene& scene, const HitInfo& hit,
		Eigen::Vector3f& lightDirection, float& pdf) const
	{
		const HitInfo primitivePoint = object->SamplePrimitivePoint(primitiveIdx);
		Eigen::Vector3f HitToLight = primitivePoint.Point - hit.Point;
		const float distance = HitToLight.norm();
		HitToLight.normalize();
		{
			const Ray lightRay { hit.Point, HitToLight, distance - 0.0001f };
			if (!TestLightVisibility(scene, hit, lightRay))
			{
				pdf = 0.f;
				return Eigen::Vector3f::Zero();
			}
		}
		lightDirection = HitToLight; // lightDirection is the direction from the reference point to the point sampled on the light
		pdf = object->PrimitiveSamplePDF(primitiveIdx); // Usually defined as 1/(ObjectArea)

		const float eFactor = primitivePoint.surfNormal.dot(-lightDirection);
		Eigen::Vector3f lightIntensity = Eigen::Vector3f::Zero();
		if (eFactor > 0.f) lightIntensity = (intensity * eFactor) / (distance * distance);

		return lightIntensity;
	}
}
