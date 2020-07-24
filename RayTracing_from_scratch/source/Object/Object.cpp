#include "pch.h"
#include "Object/Object.h"
using namespace Renderer;


Object::Object()
{
	//this->Material = nullptr;
}

Object::~Object()
{
}

bool Renderer::Object::is_bounds_hit(Ray& incoming_ray)
{
	float t0 = 0.0f, t1 = FLT_MAX;
	float tNear, tFar;
	Eigen::AlignedBox3f bound = this->Object_bounds;
	Eigen::Vector4f rayDir = incoming_ray.getDirection(), rayOrig = incoming_ray.getOrigin();

	for (int i = 0; i < 3; i++)
	{
		float invDir = 1.0f / rayDir(i);
		tNear = (bound.min()(i) - rayOrig(i)) * invDir;
		tFar = (bound.max()(i) - rayOrig(i)) * invDir;

		if (tNear > tFar) std::swap(tNear, tFar);

		t0 = tNear > t0 ? tNear : t0;
		t1 = tFar  < t1 ? tFar : t1;

		if (t0 > t1) return false;
	}

	return true;

}

void Renderer::Object::setMaterial(Renderer::Material * material)
{
	this->material = material;
}

Renderer::Material* Renderer::Object::getMaterial() const
{
	return this->material;
}

void Renderer::Object::SetBounds(Eigen::AlignedBox3f& bounds)
{
	this->Object_bounds = bounds;
}
