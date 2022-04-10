#pragma once
#include <iosfwd>
#include <iosfwd>
#include <vector>
#include <vector>
#include <Eigen/src/Geometry/AlignedBox.h>
#include <Eigen/src/Geometry/AlignedBox.h>

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
		~GroundPlane() override;

		void SetSquareSize(int size);
		std::vector<Eigen::AlignedBox3f> GetPrimitivesBounds() const override
		{
			constexpr float min = std::numeric_limits<float>::lowest();
			constexpr float max = std::numeric_limits<float>::max();
			return { Eigen::AlignedBox3f{ Eigen::Vector3f{min, Height, min}, Eigen::Vector3f{max, Height, max} } };
		}
	};
}
