#include "pch.h"
#include "Shading/RefractiveMaterial.h"
#include "RenderHeaders.h"
using namespace Renderer;


RefractiveMaterial::RefractiveMaterial(Eigen::Vector3f DiffuseCcolor, float RefractiveIndex) : Material(DiffuseCcolor), RefractiveIndex(RefractiveIndex)
{
	
}


RefractiveMaterial::~RefractiveMaterial()
{
}

Eigen::Vector3f Renderer::RefractiveMaterial::ObjectHitColor(Scene& scene, HitInfo& hit_info, int nSamples)
{
	int wantedX = 338, wantedY = 433;
	//Eigen::Vector3f DirectIllum = this->getDirectIllumination(scene, hit_info);
	Eigen::Vector3f PathTroughput = hit_info.Attenuation;
	Eigen::Vector3f Point = hit_info.Point;
	Eigen::Vector3f N = hit_info.Normal;
	Eigen::Vector3f rayDir = hit_info.ray->getDirection();
	float u = hit_info.TextureCoord.x(), v = hit_info.TextureCoord.y();
	bool isBackfaceHit = hit_info.hitBackface;

	float etai = (hit_info.ray->getRefractiveIndex() == this->RefractiveIndex)? 1.0f : hit_info.ray->getRefractiveIndex();
	float etat = this->RefractiveIndex;
	if (isBackfaceHit) std::swap(etai, etat);
	float r = etai / etat;
	//float cosCritic = cos(asinf(1 / r));
	float cosi = -hit_info.ray->getDirection().dot(hit_info.Normal);
	float sint = r * sqrtf(1.0f - cosi * cosi);
	float cost = sqrtf(1.0f - sint * sint);
	bool internalReflection = sint >= 1;
	float Fr = (internalReflection)? 1.0f : FrDieletric(cosi, cost, etai, etat, hit_info.x, hit_info.y);
	//Fr = FrDieletric(cosi, cost, etai, etat, hit_info.x, hit_info.y);

	/*if (r > 1.0f && cosi < cosCritic)
		internalReflection = true;*/

	Eigen::Vector3f rDirection, tDirection;
	rDirection = (rayDir - 2.0f * N.dot(rayDir) * N).normalized();

	

	Eigen::Vector3f IndirectIllum = Eigen::Vector3f::Zero();
	int depth = hit_info.ray->getDepth() + 1;
	delete hit_info.ray;
	if (internalReflection)
	{
		hit_info.Attenuation = this->getDiffuse(u, v).array() * PathTroughput.array();
		if (hit_info.x + 256 == 512 - wantedX && hit_info.y + 256 == 512 - wantedY)
			//#pragma omp critical
		{
			//std::cout << hit_info.x << "x" << hit_info.y << ": " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
			std::cout << "Internal Reflection Fresnel " << "; depth: " << depth - 1 << std::endl;
			std::cout << "EtaI: " << etai << std::endl;
			std::cout << "EtaT: " << etat << std::endl;
			std::cout << "D: " << hit_info.Distance << std::endl;
		}
		

		hit_info.ray = new Ray(Point, rDirection, depth, true, this->RefractiveIndex);
		IndirectIllum = scene.RayCastColor(*hit_info.ray, hit_info, nSamples);

		if (hit_info.x + 256 == 512 - wantedX && hit_info.y + 256 == 512 - wantedY)
			//#pragma omp critical
		{
			//std::cout << hit_info.x << "x" << hit_info.y << ": " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
			std::cout << "Internal Reflection back; depth: " << depth - 1 << std::endl;
		}
	}
	else
	{
		hit_info.Attenuation = Fr * this->getDiffuse(u, v).array() * PathTroughput.array();
		if (hit_info.x + 256 == 512 - wantedX && hit_info.y + 256 == 512 - wantedY)
			//#pragma omp critical
		{
			//std::cout << hit_info.x << "x" << hit_info.y << ": " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
			std::cout << "Refraction Fresnel: " << Fr << "; depth: " << depth - 1 << std::endl;
			std::cout << "EtaI: " << etai << std::endl;
			std::cout << "EtaT: " << etat << std::endl;
			std::cout << "Reflection Att: " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
		}

		

		hit_info.ray = new Ray(Point, rDirection, depth, true, etai);
		IndirectIllum = scene.RayCastColor(*hit_info.ray, hit_info, nSamples);
		delete hit_info.ray;
		
		hit_info.Attenuation = (1.0f - Fr) * this->getDiffuse(u, v).array() * PathTroughput.array();


		if (hit_info.x + 256 == 512 - wantedX && hit_info.y + 256 == 512 - wantedY)
			//#pragma omp critical
		{
			//std::cout << hit_info.x << "x" << hit_info.y << ": " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
			std::cout << "Reflection back; depth: " << depth - 1 << std::endl;
			std::cout << "Refraction Att: " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
		}
		

		tDirection = (r * rayDir + (r * cosi - cost) * N).normalized();
		hit_info.ray = new Ray(Point, tDirection, depth, true, etat);
		IndirectIllum += scene.RayCastColor(*hit_info.ray, hit_info, nSamples);

		if (hit_info.x + 256 == 512 - wantedX && hit_info.y + 256 == 512 - wantedY)
			//#pragma omp critical
		{
			//std::cout << hit_info.x << "x" << hit_info.y << ": " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
			std::cout << "Refraction back; depth: " << depth - 1 << std::endl;
		}
	}

	return IndirectIllum.array();
}

Eigen::Vector3f Renderer::RefractiveMaterial::getDirectIllumination(Scene& scene, HitInfo& hit_info)
{
	return Eigen::Vector3f::Zero();
}
