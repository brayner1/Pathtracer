#pragma once
#include "Object/Object.h"

namespace Renderer {
	class Light
	{
	protected:
		Eigen::Vector3f intensity = Eigen::Vector3f::Zero();
		Eigen::Vector3f position = Eigen::Vector3f::Zero();
		Eigen::Affine3f transform = Eigen::Affine3f::Identity();
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Light(const Eigen::Vector3f& Color);
		~Light();

		virtual Eigen::Vector3f SampleLightIntensity(const HitInfo& hit, Eigen::Vector3f& lightDirection, float& pdf) const = 0;
		virtual bool TestLightVisibility(const Scene& scene, const HitInfo& hit) const = 0;

		void SetColor(const Eigen::Vector3f& Color);
		Eigen::Vector3f getColor() const;

		void SetPosition(const Eigen::Vector3f& position);
		Eigen::Vector3f getPosition() const;

		Eigen::Matrix3f getRotation() const;
		Eigen::Vector3f getDirection() const;
	};
}
