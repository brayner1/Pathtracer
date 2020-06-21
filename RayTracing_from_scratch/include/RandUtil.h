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

inline Eigen::Vector3f random_hemisphere_vector(Eigen::Vector3f& Normal, Eigen::Vector3f& U_vector)
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

inline Eigen::Vector3f RotateVector(Eigen::Matrix4f& matrix, Eigen::Vector3f& vector)
{
	return matrix.transpose().block<3, 3>(0, 0) * vector;
}

inline float FrDieletric(const float CosI, const float CosT, const float EtaI, const float EtaT, int x = 0, int y = 0)
{

	//if (x + 256 == 512 - 346 && y + 256 == 512 - 452)
	//	//#pragma omp critical
	//{
	//	//std::cout << hit_info.x << "x" << hit_info.y << ": " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
	//	std::cout << "CosI: " << CosI << std::endl;
	//	std::cout << "CosT: " << CosT << std::endl;
	//	std::cout << "EtaI: " << EtaI << std::endl;
	//	std::cout << "EtaT: " << EtaT << std::endl;
	//}

	//float Rparl = (EtaI * CosI - EtaT * CosT) / (EtaI * CosI + EtaT * CosT);
	//float Rperp = (EtaI * CosT - EtaT * CosI) / (EtaI * CosT + EtaT * CosI);

	float Rperp = ((EtaI * CosI) - (EtaT * CosT)) / ((EtaI * CosI) + (EtaT * CosT));
	float Rparl = ((EtaT * CosI) - (EtaI * CosT)) / ((EtaT * CosI) + (EtaI * CosT));

	//if (x + 256 == 346 && y + 256 == 512 - 452)
	//	//#pragma omp critical
	//{
	//	//std::cout << hit_info.x << "x" << hit_info.y << ": " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
	//	std::cout << "Rparl: " << Rparl << std::endl;
	//	std::cout << "Rperp: " << Rperp << std::endl;
	//}

	return (Rparl * Rparl + Rperp * Rperp) / 2.0f;
}