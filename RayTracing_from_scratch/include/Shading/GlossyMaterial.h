#pragma once
#ifndef MATERIAL_GLOSSY
#define MATERIAL_GLOSSY

#include "Shading/Material.h"
namespace Renderer {
	class GlossyMaterial :
	public Material 
	{
	private:
		
		float roughness;

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

			GlossyMaterial(Eigen::Vector3f DiffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f), float Roughness = 0.0f);

		~GlossyMaterial();

		const float getRoughness() const;
		void setRoughness(float Roughness);

		Eigen::Vector3f ObjectHitColor(Scene& scene, HitInfo& hit_info);
		Eigen::Vector3f getDirectIllumination(Scene &scene, HitInfo& hit_info);



	};
}

#endif