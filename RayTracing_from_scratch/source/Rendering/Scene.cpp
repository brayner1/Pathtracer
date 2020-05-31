#include "pch.h"
#include "Rendering/Scene.h"
using namespace Renderer;

Scene::Scene(PinholeCamera mainCamera) : scene_camera(mainCamera)
{
	this->ambient_light = Eigen::Vector3f(0.3f, 0.3f, 0.3f);
	this->ambient_factor = 0.05f;
}

Scene::~Scene()
{
}

void Scene::setCamera(PinholeCamera mainCamera)
{
	this->scene_camera = mainCamera;
}

void Scene::insertObject(Object * new_object)
{
	if (new_object != nullptr)
		this->scene_objects.push_back(new_object);
}

std::vector<Renderer::Object*> Renderer::Scene::getObjects() const
{
	return this->scene_objects;
}

void Renderer::Scene::insertLight(Light * new_light)
{
	if (new_light)
		this->scene_lights.push_back(new_light);
}

std::vector<Renderer::Light*> Renderer::Scene::getLights() const
{
	return this->scene_lights;
}

const Eigen::Vector3f Renderer::Scene::getAmbientColor() const
{
	return this->ambient_light;
}

const float Renderer::Scene::getAmbientFactor() const
{
	return this->ambient_factor;
}

bool Renderer::Scene::castRay(Ray ray, HitInfo &hit)
{
	float min_dist = std::numeric_limits<float>::max();
	HitInfo closest_hit = HitInfo::resetStruct();

	bool has_hit = false;
	for (int i = 0; i < scene_objects.size(); i++) {
		HitInfo hit_info;
		if (scene_objects[i]->is_hit_by_ray(ray, hit_info)) {
			if (hit_info.Distance <= min_dist) {
				has_hit = true;
				closest_hit = hit_info;
				min_dist = hit_info.Distance;
			}
		}
	}
	hit = closest_hit;
	return has_hit;
}

bool Scene::renderSceneOnPPM(std::string out_file_path)
{
	this->scene_camera.renderSceneOnPPM(out_file_path, *this);
	return false;
}
