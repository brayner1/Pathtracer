#include "pch.h"
#include "Object/Object.h"
using namespace Renderer;

Object::~Object()
{
}

void Object::SetMaterial(std::shared_ptr<Material> material)
{
	this->material = material;
}

Material* Object::GetMaterial() const
{
	return this->material.get();
}

void Object::SetPrimitiveLight(uint32_t primitiveIndex, Light* light)
{
	if (primitiveIndex < this->GetPrimitiveCount() && light != nullptr)
	{
		primitives_lights.push_back(light);
	}
}

Light* Object::GetPrimitiveLight(uint32_t primitiveIndex) const
{
	if (primitives_lights.size() != 0 && primitiveIndex < this->GetPrimitiveCount())
	{
		return primitives_lights[primitiveIndex];
	}
	return nullptr;
}

void Object::SetBounds(const Eigen::AlignedBox3f& bounds)
{
	this->Object_bounds = bounds;
}
