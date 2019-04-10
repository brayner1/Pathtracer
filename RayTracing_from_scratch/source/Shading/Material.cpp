#include "pch.h"
#include "Shading/Material.h"
using namespace Renderer;

Renderer::Material::Material(glm::fvec3& color, glm::fvec3& specularColor, glm::fvec3& ambientColor, float Reflectivity, float Glossiness, float Roughness) :
	diffuse_color(color), specular_color(specularColor), ambient_color(ambientColor), reflectivity(Reflectivity), glossiness(Glossiness), roughness(Roughness)
{
}

const glm::fvec3 Renderer::Material::getDiffuse() const
{
	return this->diffuse_color;
}

void Renderer::Material::setDiffuse(glm::fvec3& DiffuseColor)
{
	this->diffuse_color = glm::min(glm::max(glm::fvec3(0.0f), DiffuseColor), glm::fvec3(1.0f));//DiffuseColor.cwiseMax(glm::fvec3::Zero()).cwiseMin(glm::fvec3::Ones());
}

const glm::fvec3 Renderer::Material::getSpecular() const
{
	return this->specular_color;
}

void Renderer::Material::setSpecular(glm::fvec3& SpecularColor)
{
	this->specular_color = glm::min(glm::max(glm::fvec3(0.0f), SpecularColor), glm::fvec3(1.0f));//SpecularColor.cwiseMax(glm::fvec3::Zero()).cwiseMin(glm::fvec3::Ones());
}

const glm::fvec3 Renderer::Material::getAmbient() const
{
	return this->ambient_color;
}

void Renderer::Material::setAmbient(glm::fvec3& AmbientColor)
{
	this->ambient_color = glm::min(glm::max(glm::fvec3(0.0f), AmbientColor), glm::fvec3(1.0f));//AmbientColor.cwiseMax(glm::fvec3::Zero()).cwiseMin(glm::fvec3::Ones());
}

const float Renderer::Material::getReflectivity() const
{
	return this->reflectivity;
}

void Renderer::Material::setReflectivity(float Reflectivity)
{
	this->reflectivity = std::min(std::max(Reflectivity, 0.0f), 1.0f);
}

const float Renderer::Material::getGlossiness() const
{
	return this->glossiness;
}

void Renderer::Material::setGlossiness(float Glossiness)
{
	this->glossiness = Glossiness;
}

const float Renderer::Material::getRoughness() const
{
	return this->roughness;
}

void Renderer::Material::setRoughness(float Roughness)
{
	this->roughness = Roughness;
}

Renderer::Material::~Material()
{
}
