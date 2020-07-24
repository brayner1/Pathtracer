#include "pch.h"
#include "Shading/GlossyMaterial.h"
#include "RenderHeaders.h"
using namespace Renderer;


GlossyMaterial::GlossyMaterial(Eigen::Vector3f DiffuseCcolor, float Roughness) : Material(DiffuseCcolor), roughness(Roughness)
{
	
}


GlossyMaterial::~GlossyMaterial()
{
}

const float Renderer::GlossyMaterial::getRoughness() const
{
	return this->roughness;
}

void Renderer::GlossyMaterial::setRoughness(float Roughness)
{
	this->roughness = Roughness;
}

Eigen::Vector4f Renderer::GlossyMaterial::ObjectHitColor(Scene& scene, HitInfo& hit_info, int nSamples)
{
	//Eigen::Vector3f DirectIllum = this->getDirectIllumination(scene, hit_info);
	float u = hit_info.TextureCoord.x(), v = hit_info.TextureCoord.y();
	Eigen::Vector4f pathTroughput = hit_info.Attenuation;
	Eigen::Vector4f rDirection;
	float selector = uniform_random_01();

	rDirection = (hit_info.ray->getDirection() - 2.0f * hit_info.Normal.dot(hit_info.ray->getDirection()) * hit_info.Normal).normalized();


	int depth = hit_info.ray->getDepth() + 1;
	delete hit_info.ray;
	hit_info.ray = new Ray(hit_info.Point, rDirection, depth);
	hit_info.Attenuation = pathTroughput.array() * this->getDiffuse(u, v).array();

	//Eigen::Vector3f baseColor = this->getDiffuse(hit_info.TextureCoord.x(), hit_info.TextureCoord.y());
	Eigen::Vector4f IndirectIllum = scene.RayCastColor(*hit_info.ray, hit_info, nSamples);

	return IndirectIllum;
}

Eigen::Vector4f Renderer::GlossyMaterial::getDirectIllumination(Scene& scene, HitInfo& hit_info)
{
	return Eigen::Vector4f::Zero();
}
