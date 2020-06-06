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

void Renderer::Object::setMaterial(Renderer::Material * material)
{
	this->material = material;
}

Renderer::Material* Renderer::Object::getMaterial() const
{
	return this->material;
}
