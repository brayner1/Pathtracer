#include "pch.h"
#include "Object/Object.h"
using namespace Renderer;

Object::~Object()
{
	delete material;
}

void Object::SetMaterial(Material* material)
{
	this->material = material;
}

Material* Object::getMaterial() const
{
	return this->material;
}

void Object::SetBounds(const Eigen::AlignedBox3f& bounds)
{
	this->Object_bounds = bounds;
}
