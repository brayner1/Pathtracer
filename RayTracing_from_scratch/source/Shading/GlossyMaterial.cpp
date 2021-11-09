#include "pch.h"
#include "Shading/GlossyMaterial.h"
#include "RenderHeaders.h"
using namespace Renderer;


GlossyMaterial::GlossyMaterial(Eigen::Vector3f DiffuseCcolor, float Roughness) : Material(DiffuseCcolor), roughness(Roughness)
{
	
}


//GlossyMaterial::~GlossyMaterial()
//{
//}

const float Renderer::GlossyMaterial::getRoughness() const
{
	return this->roughness;
}

void Renderer::GlossyMaterial::setRoughness(float Roughness)
{
	this->roughness = Roughness;
}

Eigen::Vector3f Renderer::GlossyMaterial::ObjectHitColor(const Ray&ray, Scene& scene, HitInfo& hit_info) const
{
	//Eigen::Vector3f DirectIllum = this->getDirectIllumination(scene, hit_info);
	float u = hit_info.TextureCoord.x(), v = hit_info.TextureCoord.y();
	Eigen::Vector3f pathTroughput = hit_info.Attenuation;
	float selector = uniform_random_01();


	const Eigen::Vector3f rDirection = (ray.getDirection() - 2.0f * hit_info.surfNormal.dot(ray.getDirection()) * hit_info.surfNormal).normalized();
	int depth = ray.getDepth() + 1;
	Ray reflected_ray = Ray(hit_info.Point, rDirection, depth);
	constexpr float pdf = 1.f;
	const float cosr = 1.f / std::abs(rDirection.dot(hit_info.surfNormal));

	hit_info.Attenuation = pathTroughput.array() * this->getDiffuse(u, v).array() * cosr * std::abs(hit_info.surfNormal.dot(rDirection)) / pdf;
	//Eigen::Vector3f baseColor = this->getDiffuse(hit_info.TextureCoord.x(), hit_info.TextureCoord.y());
	return scene.GetPathLi(reflected_ray, hit_info);
}

Eigen::Vector3f Renderer::GlossyMaterial::getDirectIllumination(const Ray& ray, const Scene& scene, const HitInfo& hit_info) const
{
	return {0.f, 0.f, 0.f};
}

Eigen::Vector3f GlossyMaterial::SampleBSDF(const Ray& outgoing_ray, const HitInfo& hit_info, Ray& inbound_ray,
	float& pdf, eSampleType& sampled_type)
{
	sampled_type = static_cast<eSampleType>(BSDF_SPECULAR | BSDF_REFLECTION);
	const Eigen::Vector3f rDirection = (outgoing_ray.getDirection() - 2.0f * hit_info.surfNormal.dot(outgoing_ray.getDirection()) * hit_info.surfNormal).normalized();
	inbound_ray = {hit_info.Point, rDirection, outgoing_ray.getDepth() + 1};
	pdf = 1.f;

	const float cosr = 1.f / std::abs(rDirection.dot(hit_info.surfNormal));
	return diffuse_color * cosr;
}
