#pragma once
namespace Renderer {
	class Light
	{
	protected:
		glm::fvec3 color;
		glm::fvec3 position;
		glm::fmat4 transform;
		//Eigen::Transform<float, 3, Eigen::Affine> transform;
	public:
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Light(glm::fvec3& Color);
		~Light();

		void setColor(float red, float green, float blue);
		void setColor(glm::fvec3& Color);
		glm::fvec3 getColor() const;

		void setPosition(const glm::fvec3& position);
		glm::fvec3 getPosition() const;

		glm::fmat3 getRotation() const;
		glm::fvec3 getDirection() const;
	};
}
