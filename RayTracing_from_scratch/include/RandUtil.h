#pragma once
#include <cstdlib>
#include <stdlib.h>
#include <random>

// Random Numbers Generation Utilities Functions

inline float uniform_random_01()
{
	static thread_local std::mt19937 generator;
	std::uniform_real_distribution<float> distribution(0.f, 1.f);

	return distribution(generator);
}

/**
 * Cosine weighted hemisphere sampling.
 * 
 * \param normal hesmiphere direction, assumeed it is normalized
 * \param u_vector support vector perpendicular to the normal, assumed it is normalized
 * \return A vector sampled in the hemisphere pointed in the direction of the normal
 */
inline Eigen::Vector3f sample_hemisphere_vector_cosine(const Eigen::Vector3f& normal, const Eigen::Vector3f& u_vector)
{
	Eigen::Vector2f u_samples{ uniform_random_01(), uniform_random_01() };
	u_samples = 2.f * u_samples - Eigen::Vector2f{ 1.f , 1.f };


	if (u_samples.x() == 0.f && u_samples.y() == 0.f)
		return normal;

	float theta, r;
	if (std::abs(u_samples.x()) > std::abs(u_samples.y()))
	{
		r = u_samples.x();
		theta = M_PI_4 * (u_samples.y() / u_samples.x());
	}
	else
	{
		r = u_samples.y();
		theta = M_PI_2 - M_PI_4 * (u_samples.x() / u_samples.y());
	}
	const float cos_theta = std::cos(theta), sin_theta = std::sin(theta);
	const float u = r * cos_theta;
	const float v = r * sin_theta;
	const float z = std::sqrt(std::max(0.f, 1.f - (u * u + v * v)));

	const Eigen::Vector3f v_vector = normal.cross(u_vector);
	return u * u_vector + v * v_vector + z * normal;
}

inline Eigen::Vector3f random_hemisphere_vector(const Eigen::Vector3f& Normal, const Eigen::Vector3f& U_vector)
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

// Geometric Utilities Functions

inline Eigen::Vector3f RotateVector(Eigen::Affine3f& transform, Eigen::Vector3f& vector)
{
	return transform.linear() * vector;
}

// Illumination Utilities Functions

inline float FrDieletric(const float CosI, const float CosT, const float EtaI, const float EtaT, int x = 0, int y = 0)
{
	const float etaI_cosI = (EtaI * CosI);
	const float etaT_cosT = (EtaT * CosT);
	const float rPerp = (etaI_cosI - etaT_cosT) / (etaI_cosI + etaT_cosT);

	const float etaT_cosI = (EtaT * CosI);
	const float etaI_cosT = (EtaI * CosT);
	const float rParl = (etaT_cosI - etaI_cosT) / (etaT_cosI + etaI_cosT);

	return (rParl * rParl + rPerp * rPerp) / 2.0f;
	/*float Rperp = ((EtaI * CosI) - (EtaT * CosT)) / ((EtaI * CosI) + (EtaT * CosT));
	float Rparl = ((EtaT * CosI) - (EtaI * CosT)) / ((EtaT * CosI) + (EtaI * CosT));

	return (Rparl * Rparl + Rperp * Rperp) / 2.0f;*/
}


// Bounding Box Utilities Functions

inline float BoundingBoxSurfaceArea(Eigen::AlignedBox3f& bound)
{
	Eigen::Vector3f size = bound.sizes();

	return 2 * (size.x() * size.y() +  size.y() * size.z() + size.z() * size.x());
}

inline bool BoundingBoxIntersect(const Renderer::Ray& incoming_ray, Eigen::Vector3f& invDir, Eigen::AlignedBox3f& BoundingBox)
{
	float t0 = 0.0f, t1 = FLT_MAX;
	float tNear, tFar;
	Eigen::Vector3f rayOrig = incoming_ray.getOrigin();

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