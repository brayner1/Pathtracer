#pragma once
namespace Renderer {
	class Ray
	{
	private:
		Eigen::Vector3f origin, direction;
		int depth;
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

			Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, int Depth = 0);
		~Ray();

		Eigen::Vector3f getOrigin() const;
		Eigen::Vector3f getDirection() const;
		const int getDepth() const;
	};

}
