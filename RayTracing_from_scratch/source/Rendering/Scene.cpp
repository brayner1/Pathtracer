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
	return has_hit;
}



Eigen::Vector3f Renderer::Scene::getPixelColor(int x, int y, int maxDepth)
{
	this->scene_camera.updateViewMatrix();
	Eigen::Vector3f rDirection = this->scene_camera.getRayDirection(x, y);
	rDirection = RotateVector(this->scene_camera.getViewMatrix(), rDirection);
	Eigen::Vector3f rPosition = this->scene_camera.getPosition();

	HitInfo closest_hit = HitInfo::resetStruct();
	closest_hit.x = x; closest_hit.y = y;
	//closest_hit.w = width; closest_hit.h = height;
	

	Eigen::Vector3f pixelColor = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	int depth = 0;
	while (true)
	{
		Ray ray = Ray(rPosition, rDirection, depth);
//		if (x % 64 == 0 || y % 64 == 0)
//#pragma omp critical
//		{
//			std::cout << x << "x" << y << ": " << closest_hit.Attenuation.x() << ", " << closest_hit.Attenuation.y() << ", " << closest_hit.Attenuation.z() << std::endl;
//		}
		bool has_hit = this->castRay(ray, closest_hit);
//		if (x + 256 % 64 == 0 || y + 256 % 64 == 0)
//#pragma omp critical
//		{
//			std::cout << x << "x" << y << ": " << closest_hit.Attenuation.x() << ", " << closest_hit.Attenuation.y() << ", " << closest_hit.Attenuation.z() << std::endl;
//		}
		if (!has_hit)
		{
			pixelColor +=  this->scene_camera.get_sky_colour(rDirection).cwiseProduct(closest_hit.Attenuation);
			break;
		}
		closest_hit.ray = &ray;
		Eigen::Vector3f radiance = closest_hit.Material->getDirectIllumination(*this, closest_hit);
		pixelColor += radiance.cwiseProduct(closest_hit.Attenuation);

		//pixelColor += closest_hit.Material->getDirectIllumination(*this, closest_hit);
		
		depth += 1;
		if (depth >= maxDepth)
		{
			float pcont = closest_hit.Attenuation.maxCoeff();
			float rnd = uniform_random_01();
			if (rnd >= pcont)
				break;
			closest_hit.Attenuation /= pcont;
		}
			

		rPosition = closest_hit.Point;
		float theta = (uniform_random_01() * 2.0f * M_PI);
		//float z = uniform_random_01();
		float r = uniform_random_01();
		float sen_phi = sqrtf(1.0f - r * r);
		//float phi = 2.0f * (float)M_PI * uniform_random_01();
		float uFactor = cosf(theta) * sen_phi;
		float vFactor = sinf(theta) * sen_phi;
		Eigen::Vector3f u, v;
		v = closest_hit.Normal.cross(closest_hit.U_vector).normalized();
		//v = closest_hit.V_vector.normalized();
		u = closest_hit.U_vector.normalized();
		//rDirection = (u * uFactor + v * vFactor + sqrtf(fmaxf(0.f, 1.0f - uFactor*uFactor - vFactor * vFactor)) * closest_hit.Normal).normalized();
		rDirection = (u * uFactor + v * vFactor + r * closest_hit.Normal).normalized();


	}
	return pixelColor;
}

bool Scene::renderSceneOnPPM(std::string out_file_path)
{
	this->scene_camera.renderSceneOnPPM(out_file_path, *this);
	return false;
}
