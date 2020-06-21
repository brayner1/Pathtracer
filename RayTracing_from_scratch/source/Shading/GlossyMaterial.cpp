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

Eigen::Vector3f Renderer::GlossyMaterial::ObjectHitColor(Scene& scene, HitInfo& hit_info, int nSamples)
{
	//Eigen::Vector3f DirectIllum = this->getDirectIllumination(scene, hit_info);
	float u = hit_info.TextureCoord.x(), v = hit_info.TextureCoord.y();
	Eigen::Vector3f pathTroughput = hit_info.Attenuation;
	Eigen::Vector3f rDirection;
	float selector = uniform_random_01();

	rDirection = (hit_info.ray->getDirection() - 2.0f * hit_info.Normal.dot(hit_info.ray->getDirection()) * hit_info.Normal).normalized();


	int depth = hit_info.ray->getDepth() + 1;
	delete hit_info.ray;
	hit_info.ray = new Ray(hit_info.Point, rDirection, depth);
	hit_info.Attenuation = pathTroughput.array() * this->getDiffuse(u, v).array();

	//Eigen::Vector3f baseColor = this->getDiffuse(hit_info.TextureCoord.x(), hit_info.TextureCoord.y());
	Eigen::Vector3f IndirectIllum = scene.RayCastColor(*hit_info.ray, hit_info, nSamples);

	return IndirectIllum;
}

Eigen::Vector3f Renderer::GlossyMaterial::getDirectIllumination(Scene& scene, HitInfo& hit_info)
{
	std::vector<Light*> scene_lights = scene.getLights();
	Eigen::Vector3f final_diffuse(0.0f, 0.0f, 0.0f);
	Eigen::Vector3f final_specular(0.0f, 0.0f, 0.0f);

	//std::cout << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
	for (size_t i = 0; i < scene_lights.size(); i++)
	{
		Eigen::Vector3f light_dir = (scene_lights[i]->getPosition() - hit_info.Point);
		float light_distance = light_dir.norm();
		light_dir.normalize();

		if (light_dir.dot(hit_info.Normal) < 0.0f)
			continue;

		//Shadow rays
		HitInfo temp;
		Ray light_ray = Ray(hit_info.Point, light_dir);
		bool is_blocked = scene.RayCast(light_ray, temp) && temp.Distance < light_distance;

		if (!is_blocked) {
			float diffCoefficient = std::fabs(light_dir.dot(hit_info.Normal));
			// Phong calculations
			float lightIntensity = 10.0f / powf(light_distance, 2);
			final_diffuse += hit_info.Attenuation.cwiseProduct(scene_lights[i]->getColor()) * diffCoefficient * lightIntensity / M_PI;

			//Eigen::Vector3f halfway = (light_dir.normalized() + hit_info.Normal.normalized()).normalized();
			//Eigen::Vector3f R = 2.0f * hit_info.Normal.dot(light_dir) * hit_info.Normal - light_dir;
			//float spec = std::fmaxf(0.0f, R.dot(-hit_info.ray->getDirection()));
			//final_specular += scene_lights[i]->getColor() * std::powf(spec, hit_info.Material->getGlossiness()) * lightIntensity;
		}
	}

	return final_diffuse.cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f)).cwiseMax(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}
