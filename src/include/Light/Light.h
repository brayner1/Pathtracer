#ifndef PTracer_Light_h
#define PTracer_Light_h
#include "Object/Object.h"

namespace Renderer {
	class Light
	{
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Light(const Eigen::Vector3f& Color);
		~Light() = default;

		virtual Eigen::Vector3f SampleLightIntensity(const Scene& scene, const HitInfo& hit, Eigen::Vector3f& lightDirection, float& pdf) const = 0;

		[[nodiscard]]
		bool IsDeltaLight() const { return isDeltaLight; }
		

		void SetColor(const Eigen::Vector3f& Color);
		Eigen::Vector3f GetColor() const;

		void SetPosition(const Eigen::Vector3f& position);
		Eigen::Vector3f getPosition() const;

		Eigen::Matrix3f getRotation() const;
		Eigen::Vector3f getDirection() const;

	protected:
		virtual bool TestLightVisibility(const Scene& scene, const HitInfo& surfHit, const Ray& lightRay) const;

		Eigen::Vector3f intensity = Eigen::Vector3f::Zero();
		Eigen::Vector3f position = Eigen::Vector3f::Zero();
		Eigen::Affine3f transform = Eigen::Affine3f::Identity();

		bool isDeltaLight = false;
	};
}

#endif
