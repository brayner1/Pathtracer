#pragma once
#include "Object/Object.h"
#include "Shading/Material.h"

namespace Renderer {
	class GroundPlane :
		public Object
	{
	private:
		bool is_checkerboard;
		float Height;
		int square_size = 2;
		Renderer::Material* Material2;

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		GroundPlane(float height = 0.0f, bool checkerboard = false);
		GroundPlane(Eigen::Vector3f color, float height = 0.0f, bool checkerboard = false);
		~GroundPlane();

		bool is_hit_by_ray(Ray incoming_ray, HitInfo& hit_info);

		void setSquareSize(int size);

	};
}
