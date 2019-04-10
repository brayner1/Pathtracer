#pragma once
#include "Object/Object.h"

namespace Renderer {

	class PinholeCamera
	{
	friend class Scene;
	friend class PhongDiffuseMaterial;
	private:
		// View Properties
		float vertical_fov, horizontal_fov, screen_aspect_ratio;
		int width, height;
		int AA_MS;

		// World Properties
		glm::fvec3 position;
		glm::fvec3 front;
		glm::fvec3 right;
		glm::fvec3 up;

		//Eigen::Transform<float, 3, Eigen::Affine> view_matrix;
		glm::fmat4 view_matrix;

		// Render Function
		bool camera_dirty;

		static glm::fvec3 get_sky_colour(glm::fvec3& ray_dir);

		bool renderSceneOnPPM(std::string out_file, Scene* scene);

		static glm::fvec3 trace(Ray* ray, Scene* scene);

	public:
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		PinholeCamera(int width = 400, int height = 400, float horizontal_field_of_view = 60.0f);
		~PinholeCamera();

		// View Functions

		const int getWidth() const;	
		const int getHeight() const;
		void setScreenSize(const int width, const int height);

		// World Functions

		glm::fvec3 getPosition() const;
		void setPosition(const glm::fvec3& new_position);
		void goForward(const float amount);

		void lookAt(glm::fvec3& target);

		void updateViewMatrix();

	};
}

