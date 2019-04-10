#pragma once
#ifndef MATERIAL_PHONG_H
#define MATERIAL_PHONG_H

#include "Shading/Material.h"
namespace Renderer {
	class PhongDiffuseMaterial :
	public Material 
	{
	public:
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		PhongDiffuseMaterial(glm::fvec3& DiffuseCcolor = glm::fvec3(0.5f, 0.5f, 0.5f),
			glm::fvec3& SpecularColor = glm::fvec3(0.5f, 0.5f, 0.5f),
			glm::fvec3& ambientColor = glm::fvec3(0.f, 0.f, 0.f),
			float Reflectivity = 0.5f,
			float Glossiness = 0.0f,
			float Roughness = 0.0f);

		~PhongDiffuseMaterial();

		glm::fvec3 get_direct_illumination(Scene* scene, HitInfo& hit_info);
		glm::fvec3 get_indirect_illumination(Scene* scene, HitInfo& hit_info);

		//Eigen::Vector4f get_hit_color(Scene scene, HitInfo& hit_info, int x, int y);

	};
}

#endif