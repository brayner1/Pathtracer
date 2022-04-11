#ifndef Ptracer_Ray_h
#define Ptracer_Ray_h
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
		Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, int Depth = 0, float MediumIndex = 1.0f, float maxDistance = std::numeric_limits<float>::max());
		Ray(Eigen::Vector3f Origin, Eigen::Vector3f Direction, float MaxDistance);

		//Eigen::Vector3f attenuation = Eigen::Vector3f::Zero();

		//Eigen::Vector3f getOrigin() const;
		const Eigen::Vector3f& getOrigin() const { return Origin; }
		const Eigen::Vector3f& getDirection() const { return Direction; }
		const int& getDepth() const { return depth; }
		const bool& getIsBackfaceHit() const { return isBackfaceHit; }
		const float& getRefractiveIndex() const { return RefractiveIndex; }
		const float& GetMaxDistance() const { return MaxDistance; }
	};

}

#endif
