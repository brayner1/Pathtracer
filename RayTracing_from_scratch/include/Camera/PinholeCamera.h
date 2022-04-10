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

		// World Properties
		Eigen::Vector3f position;
		Eigen::Vector3f front;
		Eigen::Vector3f right;
		Eigen::Vector3f up;

		// Camera transformations
		Eigen::Affine3f cam_to_world;
		Eigen::Projective3f raster_to_cam;

		// Render Function
		bool camera_dirty;

		void updateViewMatrix();
		void UpdateProjectionMatrix();
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		PinholeCamera(int width = 512, int height = 512, float horizontal_field_of_view = 60.0f);
		~PinholeCamera() = default;

		// Get the ray direction given a pixel
		Eigen::Vector3f GetRayDirection(float x, float y) const;

		// View Functions
		int GetWidth() const { return width; }	
		int GetHeight() const { return height; }
		void SetScreenSize(int width, int height);

		float GetFOV() const { return horizontal_fov; }

		// World Functions

		Eigen::Vector3f GetPosition() const;
		void SetPosition(const Eigen::Vector3f& new_position);
		void GoForward(const float amount);

		void LookAt(const Eigen::Vector3f& target);

		
		Eigen::Affine3f GetViewMatrix() const;

	};
}

