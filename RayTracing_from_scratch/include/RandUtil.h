#pragma once
#include <cstdlib>
#include <stdlib.h>
#include <random>
inline float uniform_random_01()
{
	/// RAND_MAX in <cstdlib>
	return static_cast<float>(rand()) / RAND_MAX;
	//return 0.0f;
}

inline Eigen::Vector3f RotateVector(Eigen::Matrix4f matrix, Eigen::Vector3f vector)
{
	return matrix.transpose().block<3, 3>(0, 0) * vector;
}