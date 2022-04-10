#pragma once

//#include "Object.h"
namespace Renderer {
	class Scene;
	class RGBTexture;
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

		const RGBTexture* albedoTexture = nullptr;
		bool useAlbedoTexture = false;

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		//aiMaterialProperty prop;

		Material (Eigen::Vector3f diffuseCcolor = Eigen::Vector3f(0.5f, 0.5f, 0.5f));
		Material (RGBTexture* texture);
		~Material();

		virtual Eigen::Vector3f SampleBSDF(const Eigen::Vector3f& outgoing_ray, const HitInfo& hit_info, Eigen::Vector3f& inbound_ray, float& pdf, eSampleType& sampled_type) = 0;
		virtual Eigen::Vector3f BSDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray, const HitInfo& hit_info) = 0;

		virtual float PDF(const Eigen::Vector3f& outoing_ray, const Eigen::Vector3f& inbound_ray, const HitInfo& hit_info) = 0;

		virtual float GetIor() const { return 0.f; }

		Eigen::Vector3f GetAlbedo(const float u, const float v) const;
		Eigen::Vector3f GetAlbedo(const Eigen::Vector2f& UV) const;
		void SetAlbedo(Eigen::Vector3f DiffuseColor);
	
		void SetAlbedoTexture(const RGBTexture* texture);

		const Eigen::Vector3f GetTextureColorUV(float u, float v) const;
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