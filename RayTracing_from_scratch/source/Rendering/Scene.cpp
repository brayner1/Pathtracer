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
		bool specularBounce = false;
		for(uint32_t bounces = 0; bounces < renderingMaxDepth; bounces++)
		{
			if (!RayCast(ray, hit))
			{
				L.array() += beta.array() * sky_color.array();
				break;
			}

			if (bounces == 0 || specularBounce)
			{
				// Save hit albedo and normal on the output if is the first ray
				if (bounces == 0)
				{
					outProperties.albedo = (hit.Material) ? hit.Material->GetAlbedo(hit.UvCoord) : Eigen::Vector3f::Zero();
					outProperties.normal = hit.shadNormal;
				}

				// Compute emission color for first ray or specular bounce
				Light* primLight = hit.obj->GetPrimitiveLight(hit.primitiveIndex);
				if (primLight)
				{
					if (bounces == 0)
					{
						L += hit.obj->GetPrimitiveLight(hit.primitiveIndex)->GetColor();
						break;
					}
					const float eFactor = hit.surfNormal.dot(-ray.getDirection());
					if (eFactor > 0.f)
					{
						/*L.array() += 
							(eFactor * beta.array() * hit.obj->GetPrimitiveLight(hit.primitiveIndex)->GetColor().array())
							/ (hit.obj->PrimitiveSamplePDF(hit.primitiveIndex) * hit.Distance * hit.Distance);*/
						L.array() += beta.array() * hit.obj->GetPrimitiveLight(hit.primitiveIndex)->GetColor().array();
						break;
					}
				}
			}

			if (!hit.Material) break;

			const Eigen::Vector3f outRay = -ray.getDirection();
			hit.rayDir = outRay;
			Eigen::Vector3f inboundRayDir{};
			float pdf = 0.f;
			eSampleType sampledType;

			const Eigen::Vector3f materialF = hit.Material->SampleBSDF(outRay, hit, inboundRayDir, pdf, sampledType);
			if (materialF.isZero() || pdf == 0.f) break;

			if ((sampledType & (BSDF_ALL & ~BSDF_SPECULAR)) == sampledType && scene_lights.size() > 0)
			{
				Eigen::Vector3f Li { 0.f, 0.f, 0.f };
				Li = UniformSampleOneLight(hit, *this);
				//int light_idx = uniform_random_int(0, scene_lights.size() - 1);
				//const Light* light = scene_lights[light_idx];
				////for (const Light* light : scene_lights)
				//{
				//	float lightPdf = 0.f;
				//	Eigen::Vector3f lightDir = Eigen::Vector3f::Zero();
				//	const Eigen::Vector3f lightInt = light->SampleLightIntensity(*this, hit, lightDir, lightPdf);
				//	if (!lightInt.isZero() && lightPdf > 0.f)
				//		Li += (scene_lights.size() * lightInt * abs(lightDir.dot(hit.shadNormal))) / lightPdf;
				//}
				L += (beta.array() * Li.array()).matrix();
			}

			beta.array() *= (materialF.array() * abs(inboundRayDir.dot(hit.shadNormal))) / pdf;

			ray = {hit.Point, inboundRayDir};
			specularBounce = (sampledType & BSDF_SPECULAR) != 0;

			// If a specular transmission happened, compute the eta scale for the beta computation
			if ((sampledType & (BSDF_SPECULAR | BSDF_TRANSMISSION)) == (BSDF_SPECULAR | BSDF_TRANSMISSION))
			{
				float eta = hit.Material->GetIor();
				etaScale *= (outRay.dot(hit.surfNormal) > 0.f)? eta * eta : 1 / (eta * eta);
			}

			Eigen::Vector3f rrBeta = beta * etaScale;
	        if (rrBeta.maxCoeff() < 0.8f && bounces > 3) {
	            float q = std::max(.05f, 1 - rrBeta.maxCoeff());
	            if (uniform_random_float() < q) break;
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
		OutputProperties output;
		for (int i = 0; i< nSamples; i++)
		{
			const Eigen::Vector3f rDirection = this->scene_camera.GetRayDirection(x + 0.5f, y + 0.5f);
			Ray camRay = Ray(this->scene_camera.GetPosition(), rDirection, 0);
			pixelColor += PathTrace(camRay, output) / nSamples;
			OP.albedo += output.albedo;
			OP.normal += output.normal;
		}
		OP.albedo /= nSamples;
		OP.normal /= nSamples;
		OP.color = pixelColor.array();// / (pixelColor.array() + Eigen::Array3f::Ones());// / nSamples;
	}

	uint32_t Scene::GetNumberOfVertices()
	{
		uint32_t count = 0;
		uint32_t n_objects = scene_objects.size();
		uint32_t num_vertices = 0;
		for (int i = 0; i < n_objects; ++i)
		{
			if (Renderer::Mesh* mesh = dynamic_cast<Renderer::Mesh*>(scene_objects[i]))
			{
				num_vertices += mesh->vertices.size();
			}
		}

		return num_vertices;
	}

	uint32_t Scene::GetNumberOfTriangles()
	{
		uint32_t count = 0;
		uint32_t n_objects = scene_objects.size();
		uint32_t num_triangles = 0;
		for (int i = 0; i < n_objects; ++i)
		{
			if (Renderer::Mesh* mesh = dynamic_cast<Renderer::Mesh*>(scene_objects[i]))
			{
				num_triangles += mesh->triangles.size();
			}
		}

		return num_triangles;
	}

}