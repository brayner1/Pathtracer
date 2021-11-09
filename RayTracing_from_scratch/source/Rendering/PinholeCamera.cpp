#include "pch.h"
#include "Rendering/PinholeCamera.h"
#include "RenderHeaders.h"
#include "Rendering/RenderingUtils.h"
//#include "RandUtil.h"

using namespace Renderer;

PinholeCamera::PinholeCamera(int width, int height, float horizontal_field_of_view) :
	width(width), height(height), horizontal_fov(horizontal_field_of_view)
{
	this->screen_aspect_ratio = ((float)this->width) / ((float)this->height);
	this->vertical_fov = this->horizontal_fov / this->screen_aspect_ratio;
	this->camera_dirty = false;
	lookAt(this->position + Eigen::Vector3f(0.0f, 0.0f, 1.0f));
	updateViewMatrix();
	UpdateProjectionMatrix();
}

PinholeCamera::~PinholeCamera()
{
}


Eigen::Vector3f PinholeCamera::get_sky_colour(const Eigen::Vector3f& ray_dir)
{
	return {0.f, 0.f, 0.f};
	return Eigen::Vector3f(0.051f, 0.051f, 0.051f);
	//float t = (ray_dir.y() + std::sinf((this->vertical_fov)*M_PI / 180.0f)/1.2f)/std::sinf((this->vertical_fov)*M_PI / 180.0f);
	float t = (ray_dir.y() + 1.0f) / 2.0f;
	//return Eigen::Vector3f::Zero();
	return t*Eigen::Vector3f(0.5f, 0.7f, 1.0f) + (1.0f - t)*(Eigen::Vector3f(1.0f, 1.0f, 1.0f));
	//return (t*t)*(Eigen::Vector3f(1.0f, 1.0f, 1.0f)) - (-1.0f - t)*(1.0f - t)*Eigen::Vector3f(0.5f, 0.7f, 1.0f);
}

const int PinholeCamera::getWidth() const
{
	return this->width;
}

const int PinholeCamera::getHeight() const
{
	return this->height;
}

void PinholeCamera::setScreenSize(int width, int height)
{
	this->width = width;
	this->height = height;
	this->screen_aspect_ratio = ((float)this->width) / ((float)this->height);
	this->vertical_fov = this->horizontal_fov / this->screen_aspect_ratio;
	UpdateProjectionMatrix();
}

Eigen::Vector3f PinholeCamera::getPosition() const
{
	return this->position;
}

void PinholeCamera::setPosition(const Eigen::Vector3f& new_position)
{
	this->position = new_position;

	updateViewMatrix();
}

void PinholeCamera::goForward(float amount)
{
	this->position += this->front * amount;

	updateViewMatrix();
}

void PinholeCamera::lookAt(const Eigen::Vector3f& target)
{
	this->front = (target - this->position).normalized();
	std::cout << "front vec: " << std::endl << this->front << std::endl;
	this->right = (Eigen::Vector3f(0.0, 1.0, 0.0).cross(this->front)).normalized();
	//this->right = (this->front.cross(Eigen::Vector3f(0.0, 1.0, 0.0))).normalized();
	std::cout << "right vec: " << std::endl << this->right << std::endl;
	this->up = (this->front.cross(this->right)).normalized();
	//this->up = (this->right.cross(this->front)).normalized();
	std::cout << "up vec: " << std::endl << this->up << std::endl;

	updateViewMatrix();
}

void PinholeCamera::updateViewMatrix() {
	// Calcula a matriz de visualização utilizando a matriz de mudança de coordenadas (Column-wise order)
	float matrix[16] =
	{
		-right.x(), up.x(), front.x(), position.x(),
		-right.y(), up.y(), front.y(), position.y(),
		-right.z(), up.z(), front.z(), position.z(),
		0.f, 0.f, 0.f, 1.f
	};
	//this->cam_to_world.
	this->cam_to_world = Eigen::Affine3f{ Eigen::Matrix4f(matrix).transpose() };
}

void PinholeCamera::UpdateProjectionMatrix()
{
	constexpr float far = 1000.f, near = 1e-2f;

	const float h = 1.f / std::tan((M_PI / 180.f) * horizontal_fov / 2.f);
	const float w = h * height / width;
	float mat_arr[16] =
	{
		w,		0.f,	0.f,	0.f,
		0.f,	h,		0.f,	0.f,
		0.f,	0.f,	far / (far - near), 1.f,
		0.f,	0.f,	-(far * near) / (far - near),	0.f
	};
	Eigen::Projective3f cam_to_screen = Eigen::Projective3f{ Eigen::Matrix4f{ mat_arr } };

	Eigen::Affine3f screen_to_raster
	{
		Scaling(width, height, 1.f) *
		Scaling(1.f / 2, -1.f / 2, 1.f) *
		Eigen::Translation3f (1.f, -1, 0.f)
	};

	raster_to_cam = cam_to_screen.inverse() * screen_to_raster.inverse();
}

Eigen::Vector3f PinholeCamera::getRayDirection(float x, float y)
{
	float r1 = 0.0f, r2 = 0.0f;

	Eigen::Vector4f filmPoint {x, y, 0.f, 1.f};
	Eigen::Vector4f camPoint = raster_to_cam * filmPoint;
	camPoint /= camPoint.w();
	camPoint.w() = 0.f;

	Eigen::Vector3f cam_pos { camPoint.x(), camPoint.y(), camPoint.z() };

	return (cam_to_world.linear() * cam_pos).normalized();

	// Compute Ray Direction
	return Eigen::Vector3f(
			std::sinf((this->horizontal_fov) * M_PI / 180.0f) * ((float)x + 0.5f + r1) / ((float)width),
			std::sinf((this->vertical_fov) * M_PI / 180.0f) * ((float)y + 0.5f + r2) / ((float)height),
			1.0f
		).normalized();
}

Eigen::Affine3f Renderer::PinholeCamera::getViewMatrix() const
{
	return this->cam_to_world;
}
