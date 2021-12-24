#pragma once
#include "Object/Object.h"
#include "Light/Light.h"
#include "Camera/PinholeCamera.h"
#include "Rendering/NodeHierarchy.h"

namespace Renderer {
	struct OutputProperties;

	class Scene
	{
	private:
		Eigen::Vector3f sky_color {0.f, 0.f, 0.f};
		std::vector<Object*> scene_objects;
		std::unique_ptr<NodeHierarchy> SceneRoot;
		std::vector<Light*> scene_lights;

		int renderingMaxDepth = 8;

	public:
		PinholeCamera scene_camera;
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Scene(PinholeCamera mainCamera = PinholeCamera());
		~Scene();

		void SetCamera(PinholeCamera mainCamera);

		void InsertObject(Object* new_object);
		const std::vector<Object*>& GetObjects() const;

		void BuildSceneTree();

		void InsertLight(Light* new_light);
		const std::vector<Light*>& GetLights() const;

		void SetSkyColor(const Eigen::Vector3f& skyColor) { sky_color = skyColor; }
		Eigen::Vector3f GetSkyColor() const { return sky_color; }

		bool RayCast(const Ray& ray, HitInfo &hit) const;
		float RayCast(const Ray& ray) const;        

		Eigen::Vector3f PathTrace(const Ray& camera_ray, OutputProperties& outProperties);

		void PixelColor(int x, int y, int maxDepth, int nSamples, OutputProperties &OP);
	};
}
