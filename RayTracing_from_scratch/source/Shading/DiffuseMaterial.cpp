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

Eigen::Vector3f Renderer::DiffuseMaterial::ObjectHitColor(Scene& scene, HitInfo& hit_info, int nSamples)
{
	Eigen::Vector2f texCoord = hit_info.TextureCoord;
	Eigen::Vector3f N = hit_info.Normal;
	Eigen::Vector3f U = hit_info.U_vector;
	Eigen::Vector3f P = hit_info.Point;
	Eigen::Vector3f PathTroughput = hit_info.Attenuation;

	Eigen::Vector3f DirectIllum = this->getDirectIllumination(scene, hit_info).array();
	Eigen::Vector3f IndirectIllum = Eigen::Vector3f::Zero();

	int depth = hit_info.ray->getDepth() + 1;

	for (int i = 0; i< nSamples; i++)
	{
		Eigen::Vector3f rDirection = random_hemisphere_vector(N, U);
		
		hit_info.Attenuation = this->getDiffuse(texCoord.x(), texCoord.y()).array() * PathTroughput.array();
		delete hit_info.ray;
		hit_info.ray = new Ray(P, rDirection, depth);
		
		IndirectIllum += scene.RayCastColor(*hit_info.ray, hit_info, std::max(nSamples/2, 1));
	}
	IndirectIllum /= (nSamples);

	

	return PathTroughput.array() * (DirectIllum).array() + IndirectIllum.array();
}

Eigen::Vector3f Renderer::DiffuseMaterial::getDirectIllumination(Scene& scene, HitInfo& hit_info)
{
	std::vector<Light*> scene_lights = scene.getLights();
	Eigen::Vector3f final_diffuse(0.0f, 0.0f, 0.0f);
	Eigen::Vector3f final_specular(0.0f, 0.0f, 0.0f);
	Eigen::Vector3f color;
	const float u = hit_info.TextureCoord.x(), v = hit_info.TextureCoord.y();

	/*color = hit_info.Attenuation.cwiseProduct(this->getDiffuse(u, v));

	hit_info.Attenuation = color;*/
	//std::cout << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
	for (size_t i = 0; i < scene_lights.size(); i++)
	{
		Eigen::Vector3f light_dir = (scene_lights[i]->getPosition() - hit_info.Point);
		float light_distance = light_dir.norm();
		light_dir.normalize();

		if (light_dir.dot(hit_info.Normal) < 0.0f)
			continue;

		//Shadow rays
		HitInfo temp;
		Ray light_ray = Ray(hit_info.Point, light_dir);
		bool is_blocked = scene.RayCast(light_ray, temp) && temp.Distance < light_distance;

		if (!is_blocked) {
			float diffCoefficient = fabs(light_dir.dot(hit_info.Normal));
			// Phong calculations
			//float Intensity = 40.0f / (4 * M_PI);
			float lightIntensity = 40.0f / (4 * M_PI * light_distance * light_distance);
			final_diffuse += (this->getDiffuse(u, v).array() * scene_lights[i]->getColor().array()).matrix() * diffCoefficient * lightIntensity / M_PI;

			//Eigen::Vector3f halfway = (light_dir.normalized() + hit_info.Normal.normalized()).normalized();
			//Eigen::Vector3f R = 2.0f * hit_info.Normal.dot(light_dir) * hit_info.Normal - light_dir;
			//float spec = std::fmaxf(0.0f, R.dot(-hit_info.ray->getDirection()));
			//final_specular += scene_lights[i]->getColor() * std::powf(spec, hit_info.Material->getGlossiness()) * lightIntensity;
		}
	}

	//final_diffuse += final_specular;

	//Eigen::Vector3f Color = final_diffuse;// +final_specular;
//	if (hit_info.x + 256 % 64 == 0 || hit_info.y + 256 % 64 == 0)
//#pragma omp critical
//	{
//		std::cout << hit_info.x << "x" << hit_info.y << ": " << hit_info.Attenuation.x() << ", " << hit_info.Attenuation.y() << ", " << hit_info.Attenuation.z() << std::endl;
//		std::cout << "Radiance: " << final_diffuse.x() << ", " << final_diffuse.y() << ", " << final_diffuse.z() << std::endl;
//	}

	//Eigen::Vector3f rDirection = random_hemisphere_vector(hit_info.Normal, hit_info.U_vector, hit_info.V_vector);//(u * uFactor + v * vFactor + r * hit_info.Normal).normalized();
	//int depth = hit_info.ray->getDepth() + 1;
	//delete hit_info.ray;
	//hit_info.ray = new Ray(hit_info.Point, rDirection, depth);

	return final_diffuse.cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f)).cwiseMax(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}
