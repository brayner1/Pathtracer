#include "pch.h"
#include "Shading/Material.h"
#include "RenderHeaders.h"
using namespace Renderer;

Material::Material(Eigen::Vector3f color) : diffuse_color(color)
{
}

Material::Material(RGBTexture* texture)
{
	this->SetAlbedoTexture(texture);
}

Material::~Material()
{
	if (albedoTexture)
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
	if (useAlbedoTexture)
		return this->GetTextureColorUV(u, v);

	return this->diffuse_color;
}

void Material::SetAlbedo(Eigen::Vector3f DiffuseColor)
{
	this->diffuse_color = DiffuseColor.cwiseMax(Eigen::Vector3f::Zero()).cwiseMin(Eigen::Vector3f::Ones());
}

void Renderer::Material::SetAlbedoTexture(RGBTexture* texture)
{
	if (texture)
	{
		this->albedoTexture = texture;
		this->useAlbedoTexture = true;
	}
}

const Eigen::Vector3f Renderer::Material::GetTextureColorUV(float u, float v) const
{
	Eigen::Vector3i temp = this->albedoTexture->GetColorUV(u, v);
	Eigen::Vector3f color = (Eigen::Vector3f(temp.x(), temp.y(), temp.z()) / 255.0f).array().pow(2.2f);
	return color;
}