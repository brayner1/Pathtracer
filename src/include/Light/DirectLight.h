#ifndef PTracer_DirectLight_h
#define PTracer_DirectLight_h
#include "Light.h"
namespace Renderer
{
	class DirectLight :
	public Light
	{
	public:
		explicit DirectLight(const Eigen::Vector3f& color, const Eigen::Vector3f& direction)
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


#endif