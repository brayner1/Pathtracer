#pragma once
namespace Renderer {
	class Ray
	{
	private:
		Eigen::Vector3f Origin {0.f, 0.f, 0.f}, Direction{0.f, 0.f, 0.f};
		
		int depth = 0;
		bool isBackfaceHit = false;
		float RefractiveIndex = 1.0f;
		float MaxDistance = std::numeric_limits<float>::max();
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Ray() {}
		Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, int Depth = 0, float MediumIndex = 1.0f);
		Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, float MaxDistance);

		//Eigen::Vector3f attenuation = Eigen::Vector3f::Zero();

		//Eigen::Vector3f getOrigin() const;
		inline const Eigen::Vector3f& getOrigin() const { return Origin; }
		inline const Eigen::Vector3f& getDirection() const { return Direction; }
		inline const int getDepth() const { return depth; }
		inline const bool getIsBackfaceHit() const { return isBackfaceHit; }
		inline const float getRefractiveIndex() const { return RefractiveIndex; }
	};

}
