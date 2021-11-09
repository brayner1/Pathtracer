#include "pch.h"
#include "Shading/RefractiveMaterial.h"
#include "RenderHeaders.h"
using namespace Renderer;


RefractiveMaterial::RefractiveMaterial(Eigen::Vector3f DiffuseCcolor, float RefractiveIndex) : Material(DiffuseCcolor), RefractiveIndex(RefractiveIndex)
{
	
}


//RefractiveMaterial::~RefractiveMaterial()
//= default;

Eigen::Vector3f Renderer::RefractiveMaterial::ObjectHitColor(const Ray&ray, Scene& scene, HitInfo& hit_info) const
{
	int wantedX = 338, wantedY = 433;
	//Eigen::Vector3f DirectIllum = this->getDirectIllumination(scene, hit_info);
	Eigen::Vector3f PathTroughput = hit_info.Attenuation;
	Eigen::Vector3f Point = hit_info.Point;
	const Eigen::Vector3f& N = hit_info.surfNormal;
	const Eigen::Vector3f& rayDir = ray.getDirection();
	float u = hit_info.TextureCoord.x(), v = hit_info.TextureCoord.y();
	bool isBackfaceHit = hit_info.hitBackface;

	float etai = (ray.getRefractiveIndex() == this->RefractiveIndex)? 1.0f : ray.getRefractiveIndex();
	float etat = this->RefractiveIndex;
	if (isBackfaceHit) std::swap(etai, etat);
	float eta = etai / etat;
	//float cosCritic = cos(asinf(1 / r));
	float cosi = std::abs(rayDir.dot(hit_info.surfNormal));
	if (cosi > 1.f) std::cout << "PQP: " << cosi << std::endl;
	float sint = eta * std::sqrt(std::max(0.f, 1.0f - cosi * cosi));
	float cost = std::sqrt(1.0f - sint * sint);
	bool internalReflection = sint >= 1;
	float Fr = internalReflection ? 1.f : FrDieletric(cosi, cost, etai, etat, hit_info.x, hit_info.y);
	float sample = uniform_random_01();
	
	
	Eigen::Vector3f IndirectIllum;
	int depth = ray.getDepth() + 1;
	
	if (internalReflection || sample <= Fr)
	{
		//std::cout << "sample: " << sample << std::endl;
		const Eigen::Vector3f rDirection = (rayDir - 2.0f * N.dot(rayDir) * N).normalized();
		hit_info.Attenuation = this->getDiffuse(u, v).array() * PathTroughput.array();
		const Ray reflected_ray = Ray(Point, rDirection, depth, internalReflection? this->RefractiveIndex : 1.f);
		IndirectIllum = scene.GetPathLi(reflected_ray, hit_info);
	}
	else
	{
		hit_info.Attenuation = (eta*eta) * this->getDiffuse(u, v).array() * PathTroughput.array();
		const Eigen::Vector3f forward_normal = rayDir.dot(N) > 0.f? -N: N;
		const Eigen::Vector3f tDirection = (eta * rayDir + (eta * cosi - cost) * forward_normal).normalized();
		const Ray transmitted_ray = Ray(Point, tDirection, depth, etat);
		IndirectIllum = scene.GetPathLi(transmitted_ray, hit_info);
	}

	return IndirectIllum;
}

Eigen::Vector3f Renderer::RefractiveMaterial::getDirectIllumination(const Ray& ray, const Scene& scene, const HitInfo& hit_info) const
{
	return Eigen::Vector3f::Zero();
}

Eigen::Vector3f RefractiveMaterial::SampleBSDF(const Ray& outgoing_ray, const HitInfo& hit_info, Ray& inbound_ray,
	float& pdf, eSampleType& sampled_type)
{
	bool isBackfaceHit = hit_info.hitBackface;

	float etai = (outgoing_ray.getRefractiveIndex() == this->RefractiveIndex)? 1.0f : outgoing_ray.getRefractiveIndex();
	float etat = this->RefractiveIndex;
	if (isBackfaceHit) std::swap(etai, etat);
	float r = etai / etat;
	//float cosCritic = cos(asinf(1 / r));
	float cosi = -outgoing_ray.getDirection().dot(hit_info.surfNormal);
	float sint = r * sqrtf(1.0f - cosi * cosi);
	float cost = sqrtf(1.0f - sint * sint);
	bool internalReflection = sint >= 1;
	float Fr = internalReflection ? 1.f : FrDieletric(cosi, cost, etai, etat, hit_info.x, hit_info.y);
	float sample = uniform_random_01();

	if (sample >= Fr)
	{
		sampled_type = static_cast<eSampleType>(BSDF_SPECULAR | BSDF_TRANSMISSION);
		const Eigen::Vector3f tDirection = (r * outgoing_ray.getDirection() + (r * cosi - cost) * hit_info.surfNormal).normalized();
		inbound_ray = {hit_info.Point, tDirection, outgoing_ray.getDepth() + 1, isBackfaceHit? 1.f : this->RefractiveIndex};
		pdf = 1.f - Fr;

		return diffuse_color * (1.f - Fr) * ((etai*etai)/(etat*etat)) / std::abs(tDirection.dot(hit_info.surfNormal));
	}

	sampled_type = static_cast<eSampleType>(BSDF_SPECULAR | BSDF_REFLECTION);
	const Eigen::Vector3f rDirection = (outgoing_ray.getDirection() - 2.0f * hit_info.surfNormal.dot(outgoing_ray.getDirection()) * hit_info.surfNormal).normalized();
	inbound_ray = {hit_info.Point, rDirection, outgoing_ray.getDepth() + 1, isBackfaceHit? RefractiveIndex : 1.f};
	pdf = Fr;

	const float cosr = 1.f / std::abs(rDirection.dot(hit_info.surfNormal));
	return diffuse_color * Fr * cosr;
}
