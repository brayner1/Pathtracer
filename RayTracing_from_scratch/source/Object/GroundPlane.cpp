#include "pch.h"
#include "Object/GroundPlane.h"
#include "RenderHeaders.h"
using namespace Renderer;


GroundPlane::GroundPlane(float height, bool checkerboard) : Height(height), is_checkerboard(checkerboard)
{
	this->material = new DiffuseMaterial(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
	this->Material2 = new DiffuseMaterial(Eigen::Vector3f(1.0f, 1.0f, 1.0f));
}

GroundPlane::GroundPlane(Eigen::Vector3f color, float height, bool checkerboard):  Height(height), is_checkerboard(checkerboard)
{
	//this->setColor(color);
	this->material = new DiffuseMaterial(color);
	this->Material2 = new DiffuseMaterial(Eigen::Vector3f(1.0f, 1.0f, 1.0f));
}


GroundPlane::~GroundPlane()
{
}

bool GroundPlane::is_hit_by_ray(Ray& incoming_ray, HitInfo& hit_info)
{
	if (incoming_ray.getOrigin().y() <= this->Height || incoming_ray.getDirection().y() > 0.0f) {
		return false;
	}
	hit_info.Distance = (this->Height-incoming_ray.getOrigin().y()) / incoming_ray.getDirection().y();
	float hit_point_X = incoming_ray.getOrigin().x() + incoming_ray.getDirection().x() * hit_info.Distance;
	float hit_point_Z = incoming_ray.getOrigin().z() + incoming_ray.getDirection().z() * hit_info.Distance;

	if (!is_checkerboard) {
		hit_info.Material = this->material;
		return true;
	}
	
	if ((int)std::abs(std::floorf(hit_point_X)) % (square_size * 2) < square_size) {
		if ((int)std::abs(std::floorf(hit_point_Z)) % (square_size * 2) < square_size)
			hit_info.Material = this->material;
		else
			hit_info.Material = this->Material2;
	}
	else {
		if ((int)std::abs(std::floorf(hit_point_Z)) % (square_size * 2) >= square_size)
			hit_info.Material = this->material;
		else
			hit_info.Material = this->Material2;
	}
	hit_info.Point = Eigen::Vector3f(hit_point_X, this->Height, hit_point_Z);
	hit_info.Normal = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
	hit_info.U_vector = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
	hit_info.V_vector = Eigen::Vector3f(0.0f, 0.0f, 1.0f);
	hit_info.obj = this;
	return true;

}

void GroundPlane::setSquareSize(int size)
{
	this->square_size = size;
}
