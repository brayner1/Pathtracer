#pragma once
namespace Renderer {
	class Ray
	{
	private:
		Eigen::Vector3f origin, direction;
		
		int depth;
		float lightWeight;
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

			Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, float lightWeight = 1.0f, int Depth = 0);
		~Ray();

		Eigen::Vector3f attenuation = Eigen::Vector3f::Zero();

		Eigen::Vector3f getOrigin() const;
		Eigen::Vector3f getDirection() const;
		float getAttenuation() const;
		const int getDepth() const;
	};

}
