#pragma once
namespace Renderer {
	class Ray
	{
	private:
		glm::fvec3 origin, direction;
		int depth;
		float attenuation;
	public:
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW

			Ray(glm::fvec3& Origin, glm::fvec3& Direction, float attenuation = 1.0f, int Depth = 0);
		~Ray();

		glm::fvec3 getOrigin() const;
		glm::fvec3 getDirection() const;
		float getAttenuation() const;
		const int getDepth() const;
	};

}
