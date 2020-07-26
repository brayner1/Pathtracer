#include "pch.h"
#include "Rendering/PinholeCamera.h"
#include "RenderHeaders.h"
//#include "RandUtil.h"

using namespace Renderer;

PinholeCamera::PinholeCamera(int width, int height, float horizontal_field_of_view) :
	width(width), height(height), horizontal_fov(horizontal_field_of_view)
{
	this->screen_aspect_ratio = ((float)this->width) / ((float)this->height);
	this->vertical_fov = this->horizontal_fov / this->screen_aspect_ratio;
	this->camera_dirty = false;
	lookAt(this->position + Eigen::Vector3f(0.0f, 0.0f, 1.0f));
}

PinholeCamera::~PinholeCamera()
{
}


Eigen::Vector4f PinholeCamera::get_sky_colour(const Eigen::Vector4f& ray_dir) const
{
	return Eigen::Vector4f(0.051f, 0.051f, 0.051f, 0.0f);
	//float t = (ray_dir.y() + std::sinf((this->vertical_fov)*M_PI / 180.0f)/1.2f)/std::sinf((this->vertical_fov)*M_PI / 180.0f);
	float t = (ray_dir.y() + 1.0f) / 2.0f;
	//return Eigen::Vector3f::Zero();
	return t*Eigen::Vector4f(0.5f, 0.7f, 1.0f, 0.0f) + (1.0f - t)*(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 0.0f));
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
}

Eigen::Vector4f PinholeCamera::getPosition() const
{
	Eigen::Vector3f p = this->position;
	return Eigen::Vector4f(p.x(), p.y(), p.z(), 1.0f);
}

void PinholeCamera::setPosition(Eigen::Vector3f new_position)
{
	this->camera_dirty = true;
	this->position = new_position;
}

void PinholeCamera::goForward(float amount)
{
	this->camera_dirty = true;
	this->position += this->front * amount;
}

void PinholeCamera::lookAt(Eigen::Vector3f target)
{
	this->camera_dirty = true;
	this->front = (target - this->position).normalized();
	//std::cout << "front vec: " << std::endl << this->front << std::endl;
	//this->front = (this->position - target).normalized();
	this->right = (Eigen::Vector3f(0.0, 1.0, 0.0).cross(this->front)).normalized();
	//std::cout << "right vec: " << std::endl << this->right << std::endl;
	//this->right = (this->front.cross(Eigen::Vector3f(0.0, 1.0, 0.0))).normalized();
	this->up = (this->front.cross(this->right)).normalized();
	//std::cout << "up vec: " << std::endl << this->up << std::endl;
	//this->up = (this->right.cross(this->front)).normalized();
}

void PinholeCamera::updateViewMatrix() {
	if (!this->camera_dirty)
		return;
	// Calcula a matriz de visualização utilizando a matriz de mudança de coordenadas
	const float viewMat[] = {
		this->right.x()                         , this->up.x()                         , this->front.x()                       , 0.0f,
		this->right.y()							, this->up.y()					       , this->front.y()		               , 0.0f,
		this->right.z()                         , this->up.z()					       , this->front.z()                       , 0.0f,
		-this->right.dot(this->position)		,-this->up.dot(this->position)		   , -this->front.dot(this->position)	   , 1.0f
	};

	//Eigen::Translation<float, 3> t = Eigen::Translation<float, 3>()
	//Eigen::Quaternion<float, 4> q = Eigen::Quaternion<float, 4>()
	//this->view_matrix.
	this->camera_dirty = false;
	this->view_matrix = Eigen::Matrix4f(viewMat);
	
}

Eigen::Vector4f PinholeCamera::getRayDirection(float x, float y) const
{
	float r1 = 0.0f, r2 = 0.0f;

	// Compute Ray Direction
	return Eigen::Vector4f(
			std::sinf((this->horizontal_fov) * M_PI / 180.0f) * ((float)x - 0.5f + r1) / ((float)width),
			std::sinf((this->vertical_fov) * M_PI / 180.0f) * ((float)y - 0.5f + r2) / ((float)height),
			1.0f,
			0.0f
		).normalized();
}

Eigen::Matrix4f Renderer::PinholeCamera::getViewMatrix() const
{
	return this->view_matrix;
}
