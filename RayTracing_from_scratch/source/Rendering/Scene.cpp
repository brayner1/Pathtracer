#include "pch.h"
#include "Rendering/Scene.h"
using namespace Renderer;

Scene::Scene(PinholeCamera mainCamera) : scene_camera(mainCamera)
{
	this->ambient_light = glm::fvec3(0.3f, 0.3f, 0.3f);
	this->ambient_factor = 0.0f;
	this->maxBounces = 3;
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

const glm::fvec3 Renderer::Scene::getAmbientColor() const
{
	return this->ambient_light;
}

const float Renderer::Scene::getAmbientFactor() const
{
	return this->ambient_factor;
}

const int Renderer::Scene::getMaxBounces() const
{
	return this->maxBounces;
}

bool Scene::renderSceneOnPPM(std::string out_file_path)
{
	this->scene_camera.renderSceneOnPPM(out_file_path, this);
	return false;
}
