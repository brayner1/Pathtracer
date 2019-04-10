#include "pch.h"
#include "Shading/PhongRaytracingMaterial.h"
#include "Renderer.h"
using namespace Renderer;


PhongDiffuseMaterial::PhongDiffuseMaterial(Eigen::Vector3f& DiffuseCcolor,
	Eigen::Vector3f& SpecularColor,
	Eigen::Vector3f& ambientColor,
	float Reflectivity,
	float Glossiness,
	float Roughness) : Material(DiffuseCcolor, SpecularColor, ambientColor, Reflectivity, Glossiness, Roughness)
{
	
}


PhongDiffuseMaterial::~PhongDiffuseMaterial()
{
}

Eigen::Vector3f PhongDiffuseMaterial::get_direct_illumination(Scene* scene, HitInfo& hit_info)
{
	std::vector<Light*> scene_lights = scene->getLights();
	Eigen::Vector3f final_diffuse(0.0f, 0.0f, 0.0f);
	Eigen::Vector3f final_specular(0.0f, 0.0f, 0.0f);
	for (size_t i = 0; i < scene_lights.size(); i++)
	{
		Eigen::Vector3f light_dir = (scene_lights[i]->getPosition() - hit_info.Point);
		float light_distance = light_dir.norm();
		light_dir.normalize();
		//std::vector<Object*> scene_objects = ;
		bool is_blocked = false;
		//Shadow rays
		for (size_t j = 0; j < scene->getObjects().size(); j++)
		{
			Ray* light_ray = new Ray(hit_info.Point, light_dir);
			HitInfo temp;
			if (scene->getObjects()[j]->is_hit_by_ray(light_ray, temp) && temp.Distance < light_distance) {
				is_blocked = true;
				break;
			}
			delete light_ray;
		}
		if (!is_blocked) {
			// Phong calculations
			final_diffuse += scene_lights[i]->getColor() * std::fmaxf(0.0f, light_dir.dot(hit_info.Normal));

			//Eigen::Vector3f halfway = (light_dir.normalized() + hit_info.Normal.normalized()).normalized();
			final_specular += scene_lights[i]->getColor() * std::powf(std::fmaxf(light_dir.dot(hit_info.Normal), 0.0f), hit_info.Material->getGlossiness());
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
	Eigen::Vector3f Color = hit_info.Material->getAmbient()*scene->getAmbientFactor() + 
		hit_info.Material->getDiffuse().cwiseProduct(final_diffuse) + hit_info.Material->getSpecular().cwiseProduct(final_specular);

	return Color.cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f)).cwiseMax(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}

Eigen::Vector3f Renderer::PhongDiffuseMaterial::get_indirect_illumination(Scene* scene, HitInfo & hit_info)
{
	if (hit_info.ray->getDepth() > scene->getMaxBounces() - 2)
		return Eigen::Vector3f(0.0f, 0.0f, 0.0f);

	
	Eigen::Vector3f indirect(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 32; i++) {
		float theta = (uniform_random_01() * 2.0f * M_PI);
		float z = uniform_random_01();
		float sen_phi = sqrtf(1.0f - z * z);
		Eigen::Vector3f u, v;
		v = hit_info.Normal.cross(hit_info.U_vector).normalized();
		u = hit_info.U_vector.normalized();
		Eigen::Vector3f  direction = (u*cosf(theta)*sen_phi + v * sinf(theta)*sen_phi + z * hit_info.Normal).normalized();
		Ray* reflection = new Ray(hit_info.Point,
			direction,
			1.0f,
			hit_info.ray->getDepth() + 1);
		indirect += PinholeCamera::trace(reflection, scene) * z;
		delete reflection;
	}
	return this->diffuse_color.cwiseProduct(indirect)/32.0f;
	//return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
}
