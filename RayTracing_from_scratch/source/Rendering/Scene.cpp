#include "pch.h"
#include "Rendering/Scene.h"
#include "Rendering/FrameManager.h"
#include "RenderHeaders.h"
using namespace Renderer;

namespace Renderer
{

	Scene::Scene(PinholeCamera mainCamera) : scene_camera(mainCamera)
	{
	}

	Scene::~Scene()
	{
		for (int i = 0; i < scene_objects.size(); ++i)
		{
			delete scene_objects[i];
		}

		for (int i = 0; i < scene_lights.size(); ++i)
		{
			delete scene_lights[i];
		}
	}

	void Scene::SetCamera(PinholeCamera mainCamera)
	{
		this->scene_camera = mainCamera;
	}

	void Scene::InsertObject(Object * new_object)
	{
		if (new_object != nullptr)
			this->scene_objects.push_back(new_object);
	}

	const std::vector<Object*>& Scene::GetObjects() const
	{
		return this->scene_objects;
	}

	void Scene::BuildSceneTree()
	{
		//this->SceneRoot = std::make_unique<FlatTree>(this->scene_objects);
		this->SceneRoot = std::make_unique<BVHTree>(this->scene_objects);
		//((BVHTree*)this->SceneRoot.get())->PrintTree("Log/tree_log.txt");
	}

	void Scene::InsertLight(Light * new_light)
	{
		if (new_light)
			this->scene_lights.push_back(new_light);
	}

	const std::vector<Light*>& Scene::GetLights() const
	{
		return this->scene_lights;
	}

	bool Scene::RayCast(const Ray& ray, HitInfo &hit) const
	{
		return this->SceneRoot->Intersect(ray, hit);
	}

	float Scene::RayCast(const Ray& ray) const
	{
		return this->SceneRoot->Intersect(ray);
	}

	Eigen::Vector3f Scene::PathTrace(const Ray& camera_ray, OutputProperties& outProperties)
	{
		HitInfo hit;
		Ray ray{camera_ray};
		Eigen::Vector3f beta { 1.f, 1.f, 1.f };
		Eigen::Vector3f L { 0.f, 0.f, 0.f };

		float etaScale = 1.f;
		uint32_t bounces = 0;
		for(bounces = 0; bounces < renderingMaxDepth; bounces++)
		{
			if (!RayCast(ray, hit))
			{
				L.array() += beta.array() * sky_color.array();
				break;
			}

			const Eigen::Vector3f& outRay = -ray.getDirection();
			Eigen::Vector3f inboundRayDir{};
			float pdf = 0.f;
			eSampleType sampledType;

			const Eigen::Vector3f materialF = hit.Material->SampleBSDF(outRay, hit, inboundRayDir, pdf, sampledType);
			if (materialF.isZero() || pdf == 0.f) break;

			if ((sampledType & (BSDF_ALL & ~BSDF_SPECULAR)) == sampledType)
			{
				Eigen::Vector3f Li { 0.f, 0.f, 0.f };
				for (const Light* light : scene_lights)
				{
					if (!light->TestLightVisibility(*this, hit)) continue;

					float lightPdf = 0.f;
					Eigen::Vector3f lightDir{};
					const Eigen::Vector3f lightInt = light->SampleLightIntensity(hit, lightDir, lightPdf);
					Li += (lightInt * std::abs(lightDir.dot(hit.shadNormal))) / lightPdf;
				}
				L += (beta.array() * Li.array() * materialF.array()).matrix().cwiseMin(1.f).cwiseMax(0.f);
			}

			beta = (beta.array() * materialF.array() * std::abs(inboundRayDir.dot(hit.shadNormal))) / pdf;

			/*if (beta.maxCoeff() > 1.f)
				std::cout << "deu rim: " << beta.maxCoeff() << std::endl;*/

			ray = {hit.Point, inboundRayDir};

			// If a specular transmission happened, compute the eta scale for the beta computation
			if ((sampledType & (BSDF_SPECULAR | BSDF_TRANSMISSION)) == (BSDF_SPECULAR | BSDF_TRANSMISSION))
			{
				if(RefractiveMaterial* refractiveMat = dynamic_cast<RefractiveMaterial*>(hit.Material))
				{
					float eta = refractiveMat->getIOR();
					etaScale *= (outRay.dot(hit.surfNormal) > 0.f)? eta * eta : 1 / (eta * eta);
				}
			}

			Eigen::Vector3f rrBeta = beta * etaScale;
	        if (rrBeta.maxCoeff() < 0.8f && bounces > 3) {
	            float q = std::max(.05f, 1 - rrBeta.maxCoeff());
	            if (uniform_random_01() < q) break;
	            beta /= 1 - q;
	            assert(!std::isinf(beta.y()));
	        }
		}

		return L;
	}

	void Scene::PixelColor(int x, int y, int maxDepth, int nSamples, OutputProperties &OP)
	{
		this->renderingMaxDepth = maxDepth;

		Eigen::Vector3f pixelColor {0.0f, 0.0f, 0.0f};
		for (int i = 0; i< nSamples; i++)
		{
			const Eigen::Vector3f rDirection = this->scene_camera.GetRayDirection(x + 0.5f, y + 0.5f);
			Ray camRay = Ray(this->scene_camera.GetPosition(), rDirection, 0);
			pixelColor += PathTrace(camRay, OP);
		}
		//OP.Albedo /= nSamples;
		//OP.surfNormal /= nSamples;
		OP.color = pixelColor / nSamples;
	}

}