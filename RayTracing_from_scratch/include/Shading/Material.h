#pragma once

//#include "Object.h"
namespace Renderer {
	class Scene;
	struct HitInfo;
	class Material 
	{
	protected:
		Eigen::Vector3f diffuse_color;
		Eigen::Vector3f specular_color;
		Eigen::Vector3f ambient_color;
		float reflectivity, glossiness, roughness;

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		//aiMaterialProperty prop;

		Material (Eigen::Vector3f diffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f),
			Eigen::Vector3f specularColor = Eigen::Vector3f(0.5f, 0.5f, 0.5f),
			Eigen::Vector3f ambientColor = Eigen::Vector3f(0.f, 0.f, 0.f),
			float reflectivity = 0.5f,
			float glossiness = 0.0f,
			float Roughness = 0.0f);

		virtual Eigen::Vector3f get_hit_color(Scene scene, HitInfo& hit_info) = 0;
		virtual Eigen::Vector3f getDirectIllumination(Scene& scene, HitInfo& hit_info) = 0;
		//virtual Eigen::Vector4f get_hit_color(Scene scene, HitInfo& hit_info, int x, int y) = 0;

		const Eigen::Vector3f getDiffuse() const;
		void setDiffuse(Eigen::Vector3f DiffuseColor);
		const Eigen::Vector3f getSpecular() const;
		void setSpecular(Eigen::Vector3f SpecularColor);
		const Eigen::Vector3f getAmbient() const;
		void setAmbient(Eigen::Vector3f AmbientColor);
		const float getReflectivity() const;
		void setReflectivity(float Reflectivity);
		const float getGlossiness() const;
		void setGlossiness(float Glossiness);
		const float getRoughness() const;
		void setRoughness(float Roughness);
	
		~Material();
	};
}