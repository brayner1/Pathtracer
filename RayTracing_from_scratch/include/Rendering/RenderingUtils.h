#pragma once

namespace Renderer
{
	inline Eigen::Affine3f Scaling(float x, float y, float z)
	{
		float mat_arr[16] =
		{
			x,		0.f,	0.f,	0.f,
			0.f,	y,		0.f,	0.f,
			0.f,	0.f,	z,		0.f,
			0.f,	0.f,	0.f,	1.f
		};

		return Eigen::Affine3f{ Eigen::Matrix4f{ mat_arr } };
	}
}