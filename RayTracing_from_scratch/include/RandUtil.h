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

inline Eigen::Vector3f random_hemisphere_vector(Eigen::Vector3f Normal, Eigen::Vector3f U_vector, Eigen::Vector3f V_vector)
{
	float theta = (uniform_random_01() * 2.0f * M_PI);
	float r = uniform_random_01();
	float sen_phi = sqrtf(1.0f - r * r);
	float uFactor = cosf(theta) * sen_phi;
	float vFactor = sinf(theta) * sen_phi;
	Eigen::Vector3f u, v;

	v = Normal.cross(U_vector).normalized();
	u = U_vector.normalized();
	return (u * uFactor + v * vFactor + r * Normal).normalized();
}

inline Eigen::Vector3f RotateVector(Eigen::Matrix4f matrix, Eigen::Vector3f vector)
{
	return matrix.transpose().block<3, 3>(0, 0) * vector;
}