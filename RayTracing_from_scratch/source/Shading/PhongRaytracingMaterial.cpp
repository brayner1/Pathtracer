#include "pch.h"
#include "Shading/PhongRaytracingMaterial.h"
#include "RenderHeaders.h"
using namespace Renderer;


PhongRaytracingMaterial::PhongRaytracingMaterial(Eigen::Vector3f DiffuseCcolor,
	Eigen::Vector3f SpecularColor,
	Eigen::Vector3f ambientColor,
	float Reflectivity,
	float Glossiness,
	float Roughness) : Material(DiffuseCcolor, SpecularColor, ambientColor, Reflectivity, Glossiness, Roughness)
{
	
}


PhongRaytracingMaterial::~PhongRaytracingMaterial()
{
}

Eigen::Vector3f PhongRaytracingMaterial::get_hit_color(Scene scene, HitInfo& hit_info)
{
	std::vector<Light*> scene_lights = scene.getLights();
	Eigen::Vector3f final_diffuse(0.0f, 0.0f, 0.0f);
	Eigen::Vector3f final_specular(0.0f, 0.0f, 0.0f);
	for (size_t i = 0; i < scene_lights.size(); i++)
	{
		Eigen::Vector3f light_dir = (scene_lights[i]->getPosition() - hit_info.Point);
		float light_distance = light_dir.norm();
		light_dir.normalize();
		std::vector<Object*> scene_objects = scene.getObjects();
		bool is_blocked = false;
		//Shadow rays
		for (size_t j = 0; j < scene_objects.size(); j++)
		{
			Ray light_ray = Ray(hit_info.Point, light_dir);
			HitInfo temp;
			if (scene_objects[j]->is_hit_by_ray(light_ray, temp) && temp.Distance < light_distance) {
				is_blocked = true;
				break;
			}
		}
		if (!is_blocked) {
			float diffCoefficient = std::fmaxf(0.0f, light_dir.dot(hit_info.Normal));
			// Phong calculations
			float lightIntensity = 1.0f / powf(light_distance/5.f, 2);
			final_diffuse += scene_lights[i]->getColor() * diffCoefficient * lightIntensity;

			//Eigen::Vector3f halfway = (light_dir.normalized() + hit_info.Normal.normalized()).normalized();
			Eigen::Vector3f R = 2.0f*hit_info.Normal.dot(light_dir)*hit_info.Normal - light_dir;
			float spec = std::fmaxf(0.0f, R.dot(-hit_info.ray->getDirection()));
			final_specular += scene_lights[i]->getColor() * std::powf(spec, hit_info.Material->getGlossiness()) * lightIntensity;
#if defined Debug
			if (hit_info.obj->name == "ground" && hit_info.x + hit_info.w / 2 - 1 == 282 && hit_info.y + hit_info.h / 2 - 1 == 292) {
				std::cout << "halfway: " << std::fmaxf(halfway.dot(hit_info.Normal), 0.0f) << std::endl;
			}
#endif
		}
	}
#if defined Debug
	if (hit_info.obj->name == "ground" && hit_info.x + hit_info.w/ 2 - 1 == 282 && hit_info.y + hit_info.h/ 2 - 1 == 292) {
		//std::cout << "halfway: " << std::fmaxf(halfway.dot(hit_info.Normal), 0.0f)
		std::cout << "ground final diffuse: " << final_diffuse.x() << ", " << final_diffuse.y() << ", " << final_diffuse.z() << std::endl;
		std::cout << "ground final specular: " << final_specular.x() << ", " << final_specular.y() << ", " << final_specular.z() << std::endl;
	}
#endif
	Eigen::Vector3f Color = hit_info.Material->getAmbient()*scene.getAmbientFactor() + 
		hit_info.Material->getDiffuse().cwiseProduct(final_diffuse) + hit_info.Material->getSpecular().cwiseProduct(final_specular);

	float theta = (uniform_random_01() * 2.0f * M_PI);
	float z = uniform_random_01();
	float sen_phi = sqrtf(1.0f - z*z);
	Eigen::Vector3f u, v, d;
	v = hit_info.Normal.cross(hit_info.U_vector).normalized();
	v = hit_info.V_vector;
	u = hit_info.U_vector;
	d = (u * cosf(theta) * sen_phi + v * sinf(theta) * sen_phi + z * hit_info.Normal).normalized();

	Ray reflection(hit_info.Point,
		d,
		abs(hit_info.Normal.dot(d))*hit_info.ray->getAttenuation(),
		hit_info.ray->getDepth() + 1);
	/*Ray reflection(hit_info.Point,
		((hit_info.ray->getDirection() - hit_info.Normal * hit_info.Normal.dot(hit_info.ray->getDirection())*2.0f).normalized()) +
		((Eigen::Vector3f(uniform_random_01(), uniform_random_01(), uniform_random_01()).normalized())*this->roughness).normalized(),
		1.0f,
		hit_info.ray->getDepth() + 1);*/
	//if (reflection.getDirection().norm() != 0.0f)
	hit_info.outgoing_rays.push_back(reflection);
	return Color.cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f)).cwiseMax(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}

Eigen::Vector3f Renderer::PhongRaytracingMaterial::getDirectIllumination(Scene& scene, HitInfo& hit_info)
{
	std::vector<Light*> scene_lights = scene.getLights();
	Eigen::Vector3f final_diffuse(0.0f, 0.0f, 0.0f);
	Eigen::Vector3f final_specular(0.0f, 0.0f, 0.0f);
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
			float lightIntensity = 4.0f / powf(light_distance, 2);
			final_diffuse += scene_lights[i]->getColor() * diffCoefficient * lightIntensity / M_PI;

			//Eigen::Vector3f halfway = (light_dir.normalized() + hit_info.Normal.normalized()).normalized();
			Eigen::Vector3f R = 2.0f * hit_info.Normal.dot(light_dir) * hit_info.Normal - light_dir;
			float spec = std::fmaxf(0.0f, R.dot(-hit_info.ray->getDirection()));
			final_specular += scene_lights[i]->getColor() * std::powf(spec, hit_info.Material->getGlossiness()) * lightIntensity;
		}
	}

	Eigen::Vector3f Color = final_diffuse;
//	if (hit_info.x + 256 % 64 == 0 || hit_info.y + 256 % 64 == 0)
//#pragma omp critical
//	{
//		std::cout << hit_info.x << "x" << hit_info.y << ": " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
//		std::cout << "Radiance: " << final_diffuse.x() << ", " << final_diffuse.y() << ", " << final_diffuse.z() << std::endl;
//	}
	return Color.cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f)).cwiseMax(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}
