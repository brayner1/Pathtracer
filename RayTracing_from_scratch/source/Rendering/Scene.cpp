#include "pch.h"
#include "Rendering/Scene.h"

#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>

#include "RenderHeaders.h"
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

const std::vector<Renderer::Object*>& Renderer::Scene::getObjects() const
{
	return this->scene_objects;
}

void Renderer::Scene::BuildSceneTree()
{
	//this->SceneRoot = new FlatTree(this->scene_objects);
	this->SceneRoot = new BVHTree(this->scene_objects);
	((BVHTree*)this->SceneRoot)->PrintTree("Log/tree_log.txt");
}

void Renderer::Scene::insertLight(Light * new_light)
{
	if (new_light)
		this->scene_lights.push_back(new_light);
}

const std::vector<Renderer::Light*>& Renderer::Scene::getLights() const
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

bool Renderer::Scene::RayCast(const Ray& ray, HitInfo &hit) const
{
	return this->SceneRoot->Intersect(ray, hit);
}

float Renderer::Scene::RayCast(const Ray& ray) const
{
	return this->SceneRoot->Intersect(ray);
}


Eigen::Vector3f Renderer::Scene::GetPathLi(const Ray& ray, HitInfo& hit)
{
	if (ray.getDepth() >= 3)
	{
		
		float pcont = hit.Attenuation.maxCoeff();
		float rnd = uniform_random_01();
		if (rnd >= pcont)
			return Eigen::Vector3f::Zero();
		hit.Attenuation /= pcont;
	}

	if (ray.getDepth() >= this->renderingMaxDepth)
	{
		return Eigen::Vector3f::Zero();
	}

	Eigen::Vector3f luminosity;
	if (this->RayCast(ray, hit))
	{
		luminosity = hit.Material->ObjectHitColor(ray, *this, hit);
	}
	else
	{
		luminosity = this->scene_camera.get_sky_colour(ray.getDirection()).cwiseProduct(hit.Attenuation);
	}
	return luminosity;
}

void Renderer::Scene::PixelColor(int x, int y, int maxDepth, int nSamples, struct OutputProperties &OP)
{
	this->renderingMaxDepth = maxDepth;
	

	Eigen::Vector3f pixelColor = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	for (int i = 0; i< nSamples; i++)
	{
		HitInfo closest_hit = HitInfo::resetStruct();
		closest_hit.x = x; closest_hit.y = y;
		const Eigen::Vector3f rDirection = this->scene_camera.getRayDirection(x + 0.5f, y + 0.5f);
		Ray camRay = Ray(this->scene_camera.getPosition(), rDirection, 0);
		if (this->RayCast(camRay, closest_hit))
		{
			OP.Albedo = closest_hit.Material->getDiffuse(closest_hit.TextureCoord.x(), closest_hit.TextureCoord.y());
			OP.Normal = closest_hit.surfNormal;
			pixelColor += closest_hit.Material->ObjectHitColor(camRay, *this, closest_hit);
		}
		else 
		{
			pixelColor += this->scene_camera.get_sky_colour(camRay.getDirection()).cwiseProduct(closest_hit.Attenuation);
		}
	}
	//OP.Albedo /= nSamples;
	//OP.surfNormal /= nSamples;
	OP.Color = pixelColor / nSamples;
}
