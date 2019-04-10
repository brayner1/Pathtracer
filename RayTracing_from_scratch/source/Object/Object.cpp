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

void Renderer::Object::setColor(glm::fvec3 color)
{
	this->Color = color;
}

void Renderer::Object::setMaterial(Renderer::Material * material)
{
	this->Material = material;
}
