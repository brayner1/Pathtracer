#pragma once
namespace Renderer {
	class Ray
	{
	private:
		Eigen::Vector4f Origin, Direction;
		
		int depth = 0;
		bool isBackfaceHit = false;
		float RefractiveIndex = 1.0f;
		float MaxDistance = std::numeric_limits<float>::max();
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Ray(Eigen::Vector4f Origin, Eigen::Vector4f Direction, int Depth = 0, bool BackfaceHit = false, float MediumIndex = 1.0f);
		Ray(Eigen::Vector4f Origin, Eigen::Vector4f Direction, float MaxDistance);
		~Ray();

		//Eigen::Vector3f attenuation = Eigen::Vector3f::Zero();

		const Eigen::Vector4f& getOrigin() const;
		const Eigen::Vector4f& getDirection() const;
		const int getDepth() const;
		const bool getIsBackfaceHit() const;
		const float getRefractiveIndex() const;

		//void setMaxDistance(float maxT) { MaxDistance = maxT; }
		const float getMaxDistance() const { return MaxDistance; }
	};

}
