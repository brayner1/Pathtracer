#include "pch.h"
#include "Shading/DiffuseMaterial.h"
#include "RenderHeaders.h"
using namespace Renderer;


DiffuseMaterial::DiffuseMaterial(Eigen::Vector3f DiffuseCcolor) : Material(DiffuseCcolor)
{
}

Eigen::Vector3f DiffuseMaterial::ObjectHitColor(const Ray& ray, Scene& scene, HitInfo& hit_info) const
{
	Eigen::Vector2f texCoord = hit_info.TextureCoord;
	Eigen::Vector3f N = hit_info.surfNormal;
	Eigen::Vector3f U = hit_info.U_vector;
	const Eigen::Vector3f P = hit_info.Point;

	Eigen::Vector3f DirectIllum = hit_info.Attenuation.array() * this->getDirectIllumination(ray, scene, hit_info).array();

	int depth = ray.getDepth() + 1;

	const Eigen::Vector3f rDirection = sample_hemisphere_vector_cosine(N, U);
	float pdf = std::abs(rDirection.dot(hit_info.surfNormal)) * M_1_PI;


	hit_info.Attenuation = hit_info.Attenuation.array() * this->getDiffuse(texCoord.x(), texCoord.y()).array() * M_1_PI * std::abs(rDirection.dot(hit_info.surfNormal)) / pdf;
	Ray reflected_ray = Ray(P, rDirection, depth);

	return DirectIllum + scene.GetPathLi(reflected_ray, hit_info);
}

Eigen::Vector3f DiffuseMaterial::getDirectIllumination(const Ray& ray, const Scene& scene, const HitInfo& hit_info) const
{
	const std::vector<Light*>& scene_lights = scene.getLights();

	Eigen::Vector3f final_diffuse(0.0f, 0.0f, 0.0f);
	const float& u = hit_info.TextureCoord.x(), &v = hit_info.TextureCoord.y();

	for (size_t i = 0; i < scene_lights.size(); i++)
	{
		Eigen::Vector3f light_dir = (scene_lights[i]->getPosition() - hit_info.Point);
		const float light_distance = light_dir.norm();
		light_dir.normalize();

		//Shadow ray
		Ray light_ray = Ray(hit_info.Point, light_dir);
		const float hit_dist = scene.RayCast(light_ray);
		const bool is_blocked = hit_dist > 0.f && hit_dist < light_distance;

		if (!is_blocked) {
			float diffCoefficient = fabs(light_dir.dot(hit_info.surfNormal));
			Eigen::Vector3f lightIntensity =  scene_lights[i]->getColor() / (light_distance * light_distance);
			Eigen::Vector3f diffuse = this->getDiffuse(u, v) * M_1_PI;
			final_diffuse += (diffuse.array() * lightIntensity.array()).matrix() * diffCoefficient;
		}
	}

	return final_diffuse.cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f)).cwiseMax(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
}

Eigen::Vector3f DiffuseMaterial::SampleBSDF(const Ray& outgoing_ray, const HitInfo& hit_info, Ray& inbound_ray,
                                            float& pdf, eSampleType& sampled_type)
{
	sampled_type = static_cast<eSampleType>(BSDF_DIFFUSE | BSDF_REFLECTION);
	const Eigen::Vector3f rDirection = sample_hemisphere_vector_cosine(hit_info.surfNormal, hit_info.U_vector);
	inbound_ray = {hit_info.Point, rDirection, outgoing_ray.getDepth() + 1};
	pdf = std::abs(rDirection.dot(hit_info.surfNormal)) * M_1_PI;

	return diffuse_color * M_1_PI;
}
