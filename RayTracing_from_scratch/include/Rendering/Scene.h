#pragma once
#include "Object/Object.h"
#include "Rendering/Light.h"
#include "Rendering/PinholeCamera.h"

namespace Renderer {
	class Scene
	{
	private:
		Eigen::Vector3f ambient_light;
		float ambient_factor;
		int maxBounces;
		std::vector<Object*> scene_objects;
		std::vector<Light*> scene_lights;
		PinholeCamera scene_camera;

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Scene(PinholeCamera mainCamera = PinholeCamera());
		~Scene();

		void setCamera(PinholeCamera mainCamera);

		void insertObject(Renderer::Object* new_object);
		std::vector<Renderer::Object*> getObjects() const;

		void insertLight(Light* new_light);
		std::vector<Light*> getLights() const;

		const Eigen::Vector3f getAmbientColor() const;
		const float getAmbientFactor() const;

		const int getMaxBounces() const;

		bool renderSceneOnPPM(std::string out_file);
	};
}
