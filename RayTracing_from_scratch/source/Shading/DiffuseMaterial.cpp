#include "pch.h"
#include "Shading/DiffuseMaterial.h"
#include "RenderHeaders.h"
using namespace Renderer;


DiffuseMaterial::DiffuseMaterial(Eigen::Vector3f DiffuseCcolor,
	Eigen::Vector3f SpecularColor,
	Eigen::Vector3f ambientColor,
	float Reflectivity,
	float Glossiness,
	float Roughness) : Material(DiffuseCcolor, SpecularColor, ambientColor, Reflectivity, Glossiness, Roughness)
{
	
}


DiffuseMaterial::~DiffuseMaterial()
{
}

Eigen::Vector3f Renderer::DiffuseMaterial::getDirectIllumination(Scene& scene, HitInfo& hit_info)
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
			Eigen::Vector3f R = 2.0f * hit_info.Normal.dot(light_dir) * hit_info.Normal - light_dir;
			float spec = std::fmaxf(0.0f, R.dot(-hit_info.ray->getDirection()));
			final_specular += scene_lights[i]->getColor() * std::powf(spec, hit_info.Material->getGlossiness()) * lightIntensity;
		}
	}

	Eigen::Vector3f Color = final_diffuse;// +final_specular;
//	if (hit_info.x + 256 % 64 == 0 || hit_info.y + 256 % 64 == 0)
//#pragma omp critical
//	{
//		std::cout << hit_info.x << "x" << hit_info.y << ": " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
//		std::cout << "Radiance: " << final_diffuse.x() << ", " << final_diffuse.y() << ", " << final_diffuse.z() << std::endl;
//	}

	float theta = (uniform_random_01() * 2.0f * M_PI);
	float r = uniform_random_01();
	float sen_phi = sqrtf(1.0f - r * r);
	float uFactor = cosf(theta) * sen_phi;
	float vFactor = sinf(theta) * sen_phi;
	Eigen::Vector3f u, v;
	v = hit_info.Normal.cross(hit_info.U_vector).normalized();
	u = hit_info.U_vector.normalized();
	Eigen::Vector3f rDirection = (u * uFactor + v * vFactor + r * hit_info.Normal).normalized();
	Ray* temp = hit_info.ray;
	hit_info.ray = new Ray(hit_info.Point, rDirection, temp->getDepth() + 1);
	delete temp;

	return Color.cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f)).cwiseMax(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}
