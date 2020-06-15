#pragma once

//#include "Object.h"
namespace Renderer {
	class Scene;
	class Texture;
	struct HitInfo;
	class Material 
	{
	protected:
		Eigen::Vector3f diffuse_color;
		/*Eigen::Vector3f specular_color;
		Eigen::Vector3f ambient_color;
		float reflectivity, glossiness, roughness;*/

		Texture* albedoTexture;
		bool useAlbedo = false;

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		//aiMaterialProperty prop;

		Material (Eigen::Vector3f diffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f));

		virtual Eigen::Vector3f ObjectHitColor(Scene& scene, HitInfo& hit_info) = 0;
		virtual Eigen::Vector3f getDirectIllumination(Scene& scene, HitInfo& hit_info) = 0;

		const Eigen::Vector3f getDiffuse();
		const Eigen::Vector3f getDiffuse(float u, float v);
		void setDiffuse(Eigen::Vector3f DiffuseColor);
		//const Eigen::Vector3f getSpecular() const;
		//void setSpecular(Eigen::Vector3f SpecularColor);
		//const Eigen::Vector3f getAmbient() const;
		//void setAmbient(Eigen::Vector3f AmbientColor);
		//const float getReflectivity() const;
		//void setReflectivity(float Reflectivity);
		//const float getGlossiness() const;
		//void setGlossiness(float Glossiness);
		//const float getRoughness() const;
		//void setRoughness(float Roughness);
	
		void setAlbedoTexture(Texture* texture);

		const Eigen::Vector3f getTextureColorUV(float u, float v);

		~Material();
	};
}