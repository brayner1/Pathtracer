#pragma once
#include <Unsupported/Eigen/AlignedVector3>
#include <Unsupported/Eigen/AlignedVector3>

#include "Object/Object.h"
#include "Rendering/Light.h"
#include "Rendering/PinholeCamera.h"
#include "Rendering/RenderManager.h"
#include "Rendering/ObjectTree.h"

namespace Renderer {
	class Scene
	{
	private:
		Eigen::Vector3f ambient_light;
		float ambient_factor;
		std::vector<Object*> scene_objects;
		ObjectTree* SceneRoot;
		std::vector<Light*> scene_lights;

		int renderingMaxDepth = 8;

	public:
		PinholeCamera scene_camera;
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Scene(PinholeCamera mainCamera = PinholeCamera());
		~Scene();

		void setCamera(PinholeCamera mainCamera);

		void insertObject(Renderer::Object* new_object);
		const std::vector<Renderer::Object*>& getObjects() const;

		void BuildSceneTree();

		void insertLight(Light* new_light);
		const std::vector<Renderer::Light*>& getLights() const;

		const Eigen::Vector3f getAmbientColor() const;
		const float getAmbientFactor() const;

		bool RayCast(const Ray& ray, HitInfo &hit) const;
		float RayCast(const Ray& ray) const;

		Eigen::Vector3f GetPathLi(const Ray& ray, HitInfo& hit);

		void PixelColor(int x, int y, int maxDepth, int nSamples, struct OutputProperties &OP);
	};
}
