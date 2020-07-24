#include "pch.h"
#include "Shading/DiffuseMaterial.h"
#include "RenderHeaders.h"
using namespace Renderer;


DiffuseMaterial::DiffuseMaterial(Eigen::Vector3f DiffuseCcolor) : Material(DiffuseCcolor)
{
	
}


DiffuseMaterial::~DiffuseMaterial()
{
}

Eigen::Vector4f Renderer::DiffuseMaterial::ObjectHitColor(Scene& scene, HitInfo& hit_info, int nSamples)
{
	Eigen::Vector2f texCoord = hit_info.TextureCoord;
	Eigen::Vector4f N = hit_info.Normal;
	Eigen::Vector4f U = hit_info.U_vector;
	Eigen::Vector4f P = hit_info.Point;
	Eigen::Vector4f PathTroughput = hit_info.Attenuation;

	Eigen::Vector4f DirectIllum = this->getDirectIllumination(scene, hit_info).array();
	Eigen::Vector4f IndirectIllum = Eigen::Vector4f::Zero();

	int depth = hit_info.ray->getDepth() + 1;

	for (int i = 0; i< nSamples; i++)
	{
		Eigen::Vector4f rDirection = random_hemisphere_vector(N, U);
		
		hit_info.Attenuation = this->getDiffuse(texCoord.x(), texCoord.y()).array() * PathTroughput.array();
		delete hit_info.ray;
		hit_info.ray = new Ray(P, rDirection, depth);
		
		IndirectIllum += scene.RayCastColor(*hit_info.ray, hit_info, std::max(nSamples/2, 1));
	}
	IndirectIllum /= (nSamples);

	

	return PathTroughput.array() * (DirectIllum).array() + IndirectIllum.array();
}

Eigen::Vector4f Renderer::DiffuseMaterial::getDirectIllumination(Scene& scene, HitInfo& hit_info)
{
	std::vector<Light*> scene_lights = scene.getLights();
	Eigen::Vector4f final_diffuse(0.0f, 0.0f, 0.0f, 0.0f);
	//Eigen::Vector3f final_specular(0.0f, 0.0f, 0.0f);
	//Eigen::Vector3f color;
	const float u = hit_info.TextureCoord.x(), v = hit_info.TextureCoord.y();

	//std::cout << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
	for (size_t i = 0; i < scene_lights.size(); i++)
	{
		Eigen::Vector4f light_dir = (scene_lights[i]->getPosition() - hit_info.Point);
		float light_distance = light_dir.norm();
		light_dir.normalize();

		if (light_dir.dot(hit_info.Normal) < 0.0f)
			continue;

		//Shadow rays
		Ray light_ray = Ray(hit_info.Point, light_dir, light_distance);
		bool is_blocked = scene.ShadowRayCast(light_ray);

		if (!is_blocked) {
			float diffCoefficient = fabs(light_dir.dot(hit_info.Normal));
			// Phong calculations
			//float Intensity = 40.0f / (4 * M_PI);
			float lightIntensity = 40.0f / (4 * M_PI * light_distance * light_distance);
			final_diffuse += (this->getDiffuse(u, v).array() * scene_lights[i]->getColor().array()).matrix() * diffCoefficient * lightIntensity / M_PI;
		}
	}

	return final_diffuse.cwiseMin(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 0.0f)).cwiseMax(Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
}
