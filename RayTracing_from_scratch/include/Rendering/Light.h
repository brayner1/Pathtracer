#pragma once
namespace Renderer {
	class Light
	{
	protected:
		Eigen::Vector3f color;
		Eigen::Vector3f position;
		Eigen::Transform<float, 3, Eigen::Affine> transform;
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Light(Eigen::Vector3f Color);
		~Light();

		void setColor(float red, float green, float blue);
		void setColor(Eigen::Vector3f Color);
		Eigen::Vector3f getColor() const;

		void setPosition(const Eigen::Vector3f position);
		Eigen::Vector3f getPosition() const;

		Eigen::Matrix3f getRotation() const;
		Eigen::Vector3f getDirection() const;
	};
}
