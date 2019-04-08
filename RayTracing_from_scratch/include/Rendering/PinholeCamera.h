#pragma once
#include "Object/Object.h"

namespace Renderer {

	class PinholeCamera
	{
	friend class Scene;
	private:
		// View Properties
		float vertical_fov, horizontal_fov, screen_aspect_ratio;
		int width, height;
		int AA_MS;

		// World Properties
		Eigen::Vector3f position;
		Eigen::Vector3f front;
		Eigen::Vector3f right;
		Eigen::Vector3f up;

		//Eigen::Transform<float, 3, Eigen::Affine> view_matrix;
		Eigen::Matrix4f view_matrix;

		// Render Function
		bool camera_dirty;
		int maxBounces;

		Eigen::Vector3f get_sky_colour(Eigen::Vector3f ray_dir);

		bool renderSceneOnPPM(std::string out_file, Scene scene);//std::vector<Object*> scene_objects);

		Eigen::Vector3f trace(Ray ray, Scene scene);

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		PinholeCamera(int width = 512, int height = 512, float horizontal_field_of_view = 60.0f);
		~PinholeCamera();

		// View Functions

		const int getWidth() const;	
		const int getHeight() const;
		void setScreenSize(const int width, const int height);

		// World Functions

		Eigen::Vector3f getPosition() const;
		void setPosition(const Eigen::Vector3f new_position);
		void goForward(const float amount);

		void lookAt(Eigen::Vector3f target);

		void updateViewMatrix();

	};
}

