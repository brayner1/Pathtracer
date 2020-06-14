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

Eigen::Vector3f Renderer::RefractiveMaterial::getDirectIllumination(Scene& scene, HitInfo& hit_info)
{
	std::vector<Light*> scene_lights = scene.getLights();
	Eigen::Vector3f final_diffuse(0.0f, 0.0f, 0.0f);
	Eigen::Vector3f final_specular(0.0f, 0.0f, 0.0f);
	if (useAlbedo)
		hit_info.Attenuation = hit_info.Attenuation.cwiseProduct(this->getTextureColorUV(hit_info.TextureCoord.x(), hit_info.TextureCoord.y()));
	else
		hit_info.Attenuation = hit_info.Attenuation.cwiseProduct(this->getDiffuse());

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
		bool is_blocked = scene.castRay(light_ray, temp) && temp.Distance < light_distance;

		if (!is_blocked) {
			float diffCoefficient = std::fabs(light_dir.dot(hit_info.Normal));
			// Phong calculations
			float lightIntensity = 10.0f / powf(light_distance, 2);
			final_diffuse += scene_lights[i]->getColor() * diffCoefficient * lightIntensity / M_PI;

			//Eigen::Vector3f halfway = (light_dir.normalized() + hit_info.Normal.normalized()).normalized();
			//Eigen::Vector3f R = 2.0f * hit_info.Normal.dot(light_dir) * hit_info.Normal - light_dir;
			//float spec = std::fmaxf(0.0f, R.dot(-hit_info.ray->getDirection()));
			//final_specular += scene_lights[i]->getColor() * std::powf(spec, hit_info.Material->getGlossiness()) * lightIntensity;
		}
	}

	Eigen::Vector3f Color = Eigen::Vector3f::Ones();// +final_specular;
//	if (hit_info.x + 256 % 64 == 0 || hit_info.y + 256 % 64 == 0)
//#pragma omp critical
//	{
//		std::cout << hit_info.x << "x" << hit_info.y << ": " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
//		std::cout << "Radiance: " << final_diffuse.x() << ", " << final_diffuse.y() << ", " << final_diffuse.z() << std::endl;
//	}

	float r = (!hit_info.hitBackface)? hit_info.ray->getRefractiveIndex() / this->RefractiveIndex : hit_info.ray->getRefractiveIndex();
	float cosCritic = cos(asinf(1 / r));
	float cosi = -hit_info.ray->getDirection().dot(hit_info.Normal);
	Eigen::Vector3f rDirection;
	if (r > 1.0f && cosi < cosCritic)
	{
		rDirection = (hit_info.ray->getDirection() - 2.0f * hit_info.Normal.dot(hit_info.ray->getDirection()) * hit_info.Normal).normalized();
	}
	else
	{
		float cosr = sqrtf(1.0f - r * r * (1 - cosi * cosi));
		rDirection = (r * hit_info.ray->getDirection() + (r * cosi - cosr) * hit_info.Normal).normalized();
	}
	int depth = hit_info.ray->getDepth() + 1;
	delete hit_info.ray;
	if (!hit_info.hitBackface)
		hit_info.ray = new Ray(hit_info.Point, rDirection, depth, true, hit_info.ray->getRefractiveIndex());
	else
		hit_info.ray = new Ray(hit_info.Point, rDirection, depth);

	return Color.cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f)).cwiseMax(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}
