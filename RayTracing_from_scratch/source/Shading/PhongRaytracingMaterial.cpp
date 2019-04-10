#include "pch.h"
#include "Shading/PhongRaytracingMaterial.h"
#include "Renderer.h"
using namespace Renderer;


PhongDiffuseMaterial::PhongDiffuseMaterial(glm::fvec3& DiffuseCcolor,
	glm::fvec3& SpecularColor,
	glm::fvec3& ambientColor,
	float Reflectivity,
	float Glossiness,
	float Roughness) : Material(DiffuseCcolor, SpecularColor, ambientColor, Reflectivity, Glossiness, Roughness)
{
	
}


PhongDiffuseMaterial::~PhongDiffuseMaterial()
{
}

glm::fvec3 PhongDiffuseMaterial::get_direct_illumination(Scene* scene, HitInfo& hit_info)
{
	std::vector<Light*> scene_lights = scene->getLights();
	glm::fvec3 final_diffuse(0.0f, 0.0f, 0.0f);
	glm::fvec3 final_specular(0.0f, 0.0f, 0.0f);
	for (size_t i = 0; i < scene_lights.size(); i++)
	{
		glm::fvec3 light_dir = (scene_lights[i]->getPosition() - hit_info.Point);
		float light_distance = light_dir.length();
		//light_dir.normalize();
		light_dir = glm::normalize(light_dir);
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
			final_diffuse += scene_lights[i]->getColor() * std::fmaxf(0.0f, glm::dot(light_dir, hit_info.Normal));

			//glm::fvec3 halfway = (light_dir.normalized() + hit_info.Normal.normalized()).normalized();
			final_specular += scene_lights[i]->getColor() * std::powf(std::fmaxf(glm::dot(light_dir, hit_info.Normal), 0.0f), hit_info.Material->getGlossiness());
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
	glm::fvec3 Color = hit_info.Material->getAmbient()*scene->getAmbientFactor() + 
		hit_info.Material->getDiffuse() * final_diffuse + hit_info.Material->getSpecular() * final_specular;

	//return Color.cwiseMin(glm::fvec3(1.0f, 1.0f, 1.0f)).cwiseMax(glm::fvec3(0.0f, 0.0f, 0.0f));
	return glm::max(glm::min(Color, glm::fvec3(1.0f, 1.0f, 1.0f)), glm::fvec3(0.0f, 0.0f, 0.0f));
}

glm::fvec3 Renderer::PhongDiffuseMaterial::get_indirect_illumination(Scene* scene, HitInfo & hit_info)
{
	if (hit_info.ray->getDepth() > scene->getMaxBounces() - 2)
		return glm::fvec3(0.0f, 0.0f, 0.0f);

	
	glm::fvec3 indirect(0.0f, 0.0f, 0.0f);
	int samples = 32;
	for (int i = 0; i < samples; i++) {
		float theta = (uniform_random_01() * 2.0f * M_PI);
		float z = uniform_random_01();
		float sen_phi = sqrtf(1.0f - z * z);
		glm::fvec3 u, v;
		v = glm::normalize(glm::cross(hit_info.Normal, hit_info.U_vector));//hit_info.Normal.cross(hit_info.U_vector).normalized();
		u = glm::normalize(hit_info.U_vector);//hit_info.U_vector.normalized();
		glm::fvec3  direction = glm::normalize(u*cosf(theta)*sen_phi + v * sinf(theta)*sen_phi + z * hit_info.Normal);
		Ray* reflection = new Ray(hit_info.Point,
			direction,
			1.0f,
			hit_info.ray->getDepth() + 1);
		indirect += PinholeCamera::trace(reflection, scene) * z;
		delete reflection;
	}
	return (this->diffuse_color * indirect)/((float)samples);
	//return glm::fvec3(0.0f, 0.0f, 0.0f);
}
