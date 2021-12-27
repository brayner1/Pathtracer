#pragma once

//#include "Object.h"
namespace Renderer {
	class Scene;
	class Texture;
	class Ray;
	struct HitInfo;

	enum eSampleType
	{
		BSDF_NONE = 0,
		BSDF_DIFFUSE = 1 << 0,
		BSDF_SPECULAR = 1 << 1,
		BSDF_REFLECTION = 1 << 2,
		BSDF_TRANSMISSION = 1 << 3,
		BSDF_ALL = BSDF_DIFFUSE | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION
	};

	class Material 
	{
	protected:
		Eigen::Vector3f diffuse_color;
		/*Eigen::Vector3f specular_color;
		Eigen::Vector3f ambient_color;
		float reflectivity, glossiness, roughness;*/

		Texture* albedoTexture = nullptr;
		bool useAlbedo = false;

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		//aiMaterialProperty prop;

		Material (Eigen::Vector3f diffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f));
		Material (Texture* texture);

		virtual Eigen::Vector3f SampleBSDF(const Eigen::Vector3f& outgoing_ray, const HitInfo& hit_info, Eigen::Vector3f& inbound_ray, float& pdf, eSampleType& sampled_type) = 0;

		virtual float GetIor() const { return 0.f; }

		const Eigen::Vector3f GetAlbedo() const;
		Eigen::Vector3f GetAlbedo(const float& u, const float& v) const;
		Eigen::Vector3f GetAlbedo(const Eigen::Vector2f& UV) const;
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

		const Eigen::Vector3f getTextureColorUV(float u, float v) const;

		~Material();
	};

	inline Eigen::Vector3f FaceForward(const Eigen::Vector3f& face_vector, const Eigen::Vector3f in_vector)
	{
		return face_vector.dot(in_vector) > 0.f? in_vector : - in_vector;
	}

	inline Eigen::Vector3f Reflect(const Eigen::Vector3f& in_direction, const Eigen::Vector3f& normal)
	{
		return (-in_direction + (2.f * in_direction.dot(normal) * normal)).normalized();
	}

	inline Eigen::Vector3f Refract(const Eigen::Vector3f& in_direction, const Eigen::Vector3f& normal, 
		const float cosI, const float cosT, const float eta)
	{
		return (eta * -in_direction + (eta * cosI - cosT) * normal).normalized();
	}

}