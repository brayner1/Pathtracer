#include "pch.h"
#include "Shading/Material.h"
#include "RenderHeaders.h"
using namespace Renderer;

Material::Material(Eigen::Vector3f color) : diffuse_color(color)
{
}

Material::Material(RGBTexture* texture)
{
	this->setAlbedoTexture(texture);
}

Material::~Material()
{
	delete albedoTexture;
}

const Eigen::Vector3f Material::GetAlbedo() const
{
	return this->diffuse_color;
}

Eigen::Vector3f Material::GetAlbedo(const Eigen::Vector2f& UV) const
{
	return this->GetAlbedo(UV.x(), UV.y());
}

Eigen::Vector3f Material::GetAlbedo(const float& u, const float& v) const
{
	if (useAlbedo)
		return this->getTextureColorUV(u, v);

	return this->diffuse_color;
}

void Material::setDiffuse(Eigen::Vector3f DiffuseColor)
{
	this->diffuse_color = DiffuseColor.cwiseMax(Eigen::Vector3f::Zero()).cwiseMin(Eigen::Vector3f::Ones());
}

//const Eigen::Vector3f Renderer::Material::getSpecular() const
//{
//	return this->specular_color;
//}
//
//void Renderer::Material::setSpecular(Eigen::Vector3f SpecularColor)
//{
//	this->specular_color = SpecularColor.cwiseMax(Eigen::Vector3f::Zero()).cwiseMin(Eigen::Vector3f::Ones());
//}
//
//const Eigen::Vector3f Renderer::Material::getAmbient() const
//{
//	return this->ambient_color;
//}
//
//void Renderer::Material::setAmbient(Eigen::Vector3f AmbientColor)
//{
//	this->ambient_color = AmbientColor.cwiseMax(Eigen::Vector3f::Zero()).cwiseMin(Eigen::Vector3f::Ones());
//}
//
//const float Renderer::Material::getReflectivity() const
//{
//	return this->reflectivity;
//}
//
//void Renderer::Material::setReflectivity(float Reflectivity)
//{
//	this->reflectivity = std::min(std::max(Reflectivity, 0.0f), 1.0f);
//}
//
//const float Renderer::Material::getGlossiness() const
//{
//	return this->glossiness;
//}
//
//void Renderer::Material::setGlossiness(float Glossiness)
//{
//	this->glossiness = Glossiness;
//}
//
//const float Renderer::Material::getRoughness() const
//{
//	return this->roughness;
//}
//
//void Renderer::Material::setRoughness(float Roughness)
//{
//	this->roughness = Roughness;
//}

void Renderer::Material::setAlbedoTexture(RGBTexture* texture)
{
	if (texture)
	{
		this->albedoTexture = texture;
		this->useAlbedo = true;
	}
}

const Eigen::Vector3f Renderer::Material::getTextureColorUV(float u, float v) const
{
	Eigen::Vector3i temp = this->albedoTexture->GetColorUV(u, v);
	Eigen::Vector3f color = (Eigen::Vector3f(temp.x(), temp.y(), temp.z()) / 255.0f).array().pow(2.2f);
	return color;
}