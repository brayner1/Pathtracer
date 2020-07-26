#pragma once
#include <cstdlib>
#include <stdlib.h>
#include <random>

// Random Numbers Generation Utilities Functions

inline float uniform_random_01()
{
	/// RAND_MAX in <cstdlib>
	return static_cast<float>(rand()) / RAND_MAX;
	//return 0.0f;
}

inline Eigen::Vector4f random_hemisphere_vector(Eigen::Vector4f& Normal, Eigen::Vector4f& U_vector)
{
	float theta = (uniform_random_01() * 2.0f * M_PI);
	float r = uniform_random_01();
	float sen_phi = sqrtf(1.0f - r * r);
	float uFactor = cosf(theta) * sen_phi;
	float vFactor = sinf(theta) * sen_phi;
	Eigen::Vector4f u, v;

	v = Normal.cross3(U_vector).normalized();
	u = U_vector.normalized();
	return (u * uFactor + v * vFactor + r * Normal).normalized();
}

// Geometric Utilities Functions

inline Eigen::Vector4f RotateVector(Eigen::Matrix4f& matrix, Eigen::Vector4f& vector)
{
	Eigen::Matrix4f transf = matrix.transpose();
	transf.block<1, 3>(3, 0) = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	Eigen::Vector4f r = transf * vector;
	return r;
}

// Illumination Utilities Functions

inline float FrDieletric(const float CosI, const float CosT, const float EtaI, const float EtaT, int x = 0, int y = 0)
{

	float Rperp = ((EtaI * CosI) - (EtaT * CosT)) / ((EtaI * CosI) + (EtaT * CosT));
	float Rparl = ((EtaT * CosI) - (EtaI * CosT)) / ((EtaT * CosI) + (EtaI * CosT));

	return (Rparl * Rparl + Rperp * Rperp) / 2.0f;
}


// Bounding Box Utilities Functions

inline float BoundingBoxSurfaceArea(Eigen::AlignedBox3f& bound)
{
	Eigen::Vector3f size = bound.sizes();

	return 2 * (size.x() * size.y() +  size.y() * size.z() + size.z() * size.x());
}

inline bool BoundingBoxIntersect(Renderer::Ray& incoming_ray, Eigen::Vector4f& invDir, Eigen::AlignedBox3f& BoundingBox)
{
	float t0 = 0.0f, t1 = FLT_MAX;
	float tNear, tFar;
	Eigen::Vector4f rayOrig = incoming_ray.getOrigin();

	for (int i = 0; i < 3; i++)
	{
		//float invDir = 1.0f / rayDir(i);
		tNear = (BoundingBox.min()(i) - rayOrig(i)) * invDir(i);
		tFar = (BoundingBox.max()(i) - rayOrig(i)) * invDir(i);

		if (tNear > tFar) std::swap(tNear, tFar);

		t0 = tNear > t0 ? tNear : t0;
		t1 = tFar < t1 ? tFar : t1;

		if (t0 > t1) return false;
	}

	if (t0 >= incoming_ray.getMaxDistance()) return false;

	return true;
}

inline Eigen::Vector3f BoundOffset(const Eigen::AlignedBox3f& bound, const Eigen::Vector3f& p)
{
	Eigen::Vector3f offset = p - bound.min();

	if (!bound.isEmpty())
	{
		offset = (offset.array() / (bound.max() - bound.min()).array());
	}

	return offset;
}