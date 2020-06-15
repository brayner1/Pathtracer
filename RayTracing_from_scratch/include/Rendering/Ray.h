#pragma once
namespace Renderer {
	class Ray
	{
	private:
		Eigen::Vector3f Origin, Direction;
		
		int depth = 0;
		bool isBackfaceHit = false;
		float RefractiveIndex = 1.0f;
		float MaxDistance = std::numeric_limits<float>::max();
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, int Depth = 0, bool BackfaceHit = false, float MediumIndex = 1.0f);
		Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, float MaxDistance);
		~Ray();

		Eigen::Vector3f attenuation = Eigen::Vector3f::Zero();

		Eigen::Vector3f getOrigin() const;
		Eigen::Vector3f getDirection() const;
		const int getDepth() const;
		const bool getIsBackfaceHit() const;
		const float getRefractiveIndex() const;
	};

}
