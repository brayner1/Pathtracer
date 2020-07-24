#include "pch.h"
#include "Rendering/Scene.h"
#include "RenderHeaders.h"
using namespace Renderer;

Scene::Scene(PinholeCamera mainCamera) : scene_camera(mainCamera)
{
	this->ambient_light = Eigen::Vector3f(0.3f, 0.3f, 0.3f);
	this->ambient_factor = 0.05f;
}

Scene::~Scene() {}

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

void Renderer::Scene::BuildSceneTree()
{
	//this->SceneRoot = new FlatTree(this->scene_objects);
	this->SceneRoot = new BVHTree(this->scene_objects);
	((BVHTree*)this->SceneRoot)->PrinTree();
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

bool Renderer::Scene::RayCast(Ray& ray, HitInfo &hit)
{
	return this->SceneRoot->Intersect(ray, hit);
	/*float min_dist = std::numeric_limits<float>::max();

	bool has_hit = false;
	for (int i = 0; i < scene_objects.size(); i++) {
		HitInfo hit_info = hit;
		if (scene_objects[i]->is_hit_by_ray(ray, hit_info)) {
			if (hit_info.Distance <= min_dist) {
				has_hit = true;
				hit = hit_info;
				min_dist = hit_info.Distance;
			}
		}
	}
	return has_hit;*/
}




bool Renderer::Scene::ShadowRayCast(Ray& ray)
{
	return this->SceneRoot->Intersect(ray);
}

Eigen::Vector4f Renderer::Scene::RayCastColor(Ray& ray, HitInfo& hit, int nSamples)
{
	Eigen::Vector4f luminosity = Eigen::Vector4f::Zero();

	if (hit.ray->getDepth() >= 3)
	{
		
		float pcont = hit.Attenuation.maxCoeff();
		float rnd = uniform_random_01();
		if (rnd >= pcont)
			return luminosity;
		hit.Attenuation /= pcont;
	}

	if (hit.ray->getDepth() >= this->renderingMaxDepth)
	{
		return luminosity;
	}

	if (this->RayCast(*hit.ray, hit))
	{
		luminosity = hit.Material->ObjectHitColor(*this, hit, nSamples);
	}
	else
	{
		luminosity = this->scene_camera.get_sky_colour(hit.ray->getDirection()).cwiseProduct(hit.Attenuation);
	}
	return luminosity;
}

void Renderer::Scene::PixelColor(int x, int y, int maxDepth, int nSamples, struct OutputProperties &OP)
{
	this->renderingMaxDepth = maxDepth;
	this->scene_camera.updateViewMatrix();
	Eigen::Vector4f rDirection = this->scene_camera.getRayDirection(x, y);
	rDirection = RotateVector(this->scene_camera.getViewMatrix(), rDirection);

	HitInfo closest_hit = HitInfo::resetStruct();
	closest_hit.x = x; closest_hit.y = y;
	closest_hit.Point = this->scene_camera.getPosition();
	closest_hit.ray = new Ray(closest_hit.Point, rDirection, 0);
	//closest_hit.w = width; closest_hit.h = height;
	

	Eigen::Vector4f pixelColor = Eigen::Vector4f::Zero();
	bool firstHit = true;
	int depth = 0;

	if (this->RayCast(*closest_hit.ray, closest_hit))
	{
		OP.Albedo = closest_hit.Material->getDiffuse(closest_hit.TextureCoord.x(), closest_hit.TextureCoord.y());
		OP.Normal = closest_hit.Normal;
		pixelColor = closest_hit.Material->ObjectHitColor(*this, closest_hit, nSamples);
	}
	else 
	{
		pixelColor = this->scene_camera.get_sky_colour(closest_hit.ray->getDirection()).cwiseProduct(closest_hit.Attenuation);
	}

	OP.Color = pixelColor;
}
