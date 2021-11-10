#ifndef OBJECT_H
#define OBJECT_H
#include "Rendering/Ray.h"
#include "Shading/Material.h"

namespace Renderer {
	//class Material;
	class Object;
	struct HitInfo {
		Object* obj;

		Material * Material;
		Eigen::Vector3f Point;
		Eigen::Vector3f surfNormal;
		Eigen::Vector3f shadNormal;
		Eigen::Vector2f TextureCoord;
		float U_factor, V_factor, Distance;
		Eigen::Vector3f U_vector, V_vector;
		Eigen::Vector3f Attenuation;
		Eigen::Vector3f Albedo;
		int x, y, w, h;
		bool hitBackface;

		static struct HitInfo resetStruct() {
			HitInfo info;
			info.obj = NULL;
			info.Material = NULL;
			info.Point = info.surfNormal = Eigen::Vector3f::Zero();
			info.TextureCoord = Eigen::Vector2f::Zero();
			info.U_factor = info.V_factor = info.Distance = 0.0f;
			info.Attenuation = Eigen::Vector3f::Ones();
			info.Albedo = Eigen::Vector3f::Zero();
			info.hitBackface = false;
			return info;
		}
	};

	class Object
	{
	protected:
		Material* material;
		Eigen::Vector3f Color;

		Eigen::AlignedBox3f Object_bounds;
	public:
		std::string name;

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Object();
		virtual ~Object();

		// Checks if the whole object is hit by the incoming ray, and save the hit information if it was hit.
		virtual bool isHitByRay(const Ray& incoming_ray, HitInfo& hit_info) = 0;
		// Checks if the whole object is hit by the incoming ray.
		virtual float isHitByRay(const Ray& incoming_ray) = 0;

		// Checks if a primitive in the object is hit by the incoming ray and save the hit information if it was hit.
		// If the object type don't have multiple primitives, it calculates the collision with the whole object.
		virtual float isPrimitiveHitByRay(const Ray& incoming_ray, int primitive_index, HitInfo& hit_info) = 0;
		// Checks if a primitive in the object is hit by the incoming ray.
		// If the object type don't have multiple primitives, it calculates the collision with the whole object.
		virtual float isPrimitiveHitByRay(const Ray& incoming_ray, int primitive_index) = 0;

		bool is_bounds_hit(Ray& incoming_ray);

		void setMaterial(Renderer::Material* material);
		Material* getMaterial() const;

		void SetBounds(Eigen::AlignedBox3f& bounds);
		const Eigen::AlignedBox3f& GetBounds() const { return this->Object_bounds; }
		virtual std::vector<Eigen::AlignedBox3f> GetPrimitivesBounds() = 0;
	};
}

#endif