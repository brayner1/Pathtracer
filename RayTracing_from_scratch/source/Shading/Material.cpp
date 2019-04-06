#include "pch.h"
#include "Shading/Material.h"
using namespace Renderer;

Renderer::Material::Material(Eigen::Vector3f color, Eigen::Vector3f specularColor, Eigen::Vector3f ambientColor, float Reflectivity, float Glossiness, float Roughness) :
	diffuse_color(color), specular_color(specularColor), ambient_color(ambientColor), reflectivity(Reflectivity), glossiness(Glossiness), roughness(Roughness)
{
}

const Eigen::Vector3f Renderer::Material::getDiffuse() const
{
	return this->diffuse_color;
}

void Renderer::Material::setDiffuse(Eigen::Vector3f DiffuseColor)
{
	this->diffuse_color = DiffuseColor.cwiseMax(Eigen::Vector3f::Zero()).cwiseMin(Eigen::Vector3f::Ones());
}

const Eigen::Vector3f Renderer::Material::getSpecular() const
{
	return this->specular_color;
}

void Renderer::Material::setSpecular(Eigen::Vector3f SpecularColor)
{
	this->specular_color = SpecularColor.cwiseMax(Eigen::Vector3f::Zero()).cwiseMin(Eigen::Vector3f::Ones());
}

const Eigen::Vector3f Renderer::Material::getAmbient() const
{
	return this->ambient_color;
}

void Renderer::Material::setAmbient(Eigen::Vector3f AmbientColor)
{
	this->ambient_color = AmbientColor.cwiseMax(Eigen::Vector3f::Zero()).cwiseMin(Eigen::Vector3f::Ones());
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
