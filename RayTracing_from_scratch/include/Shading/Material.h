#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

//#include "Object.h"
namespace Renderer {
	class Scene;
	struct HitInfo;
	class Material 
	{
	protected:
		glm::fvec3 diffuse_color;
		glm::fvec3 specular_color;
		glm::fvec3 ambient_color;
		float reflectivity, glossiness, roughness;

	public:
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		//aiMaterialProperty prop;

		Material (glm::fvec3& diffuseCcolor = glm::fvec3(0.5f, 0.5f, 0.5f),
			glm::fvec3& specularColor = glm::fvec3(0.5f, 0.5f, 0.5f),
			glm::fvec3& ambientColor = glm::fvec3(0.f, 0.f, 0.f),
			float reflectivity = 0.5f,
			float glossiness = 0.0f,
			float Roughness = 0.0f);

		virtual glm::fvec3 get_direct_illumination(Scene* scene, HitInfo& hit_info) = 0;
		virtual glm::fvec3 get_indirect_illumination(Scene* scene, HitInfo& hit_info) = 0;
		//virtual Eigen::Vector4f get_hit_color(Scene scene, HitInfo& hit_info, int x, int y) = 0;

		const glm::fvec3 getDiffuse() const;
		void setDiffuse(glm::fvec3& DiffuseColor);
		const glm::fvec3 getSpecular() const;
		void setSpecular(glm::fvec3& SpecularColor);
		const glm::fvec3 getAmbient() const;
		void setAmbient(glm::fvec3& AmbientColor);
		const float getReflectivity() const;
		void setReflectivity(float Reflectivity);
		const float getGlossiness() const;
		void setGlossiness(float Glossiness);
		const float getRoughness() const;
		void setRoughness(float Roughness);
	
		~Material();
	};
}

#endif
