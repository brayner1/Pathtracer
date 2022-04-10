#ifndef OBJECT_H
#define OBJECT_H
#include "Rendering/Ray.h"
#include "Shading/Material.h"

namespace Renderer {
	//class Material;
	class Object;
	class Light;
	struct NodePrimitive;

	struct HitInfo {
		const Object* obj = nullptr;
		Material * Material = nullptr;

		Eigen::Vector3f rayDir = Eigen::Vector3f::Zero();
		Eigen::Vector3f Point = Eigen::Vector3f::Zero();
		Eigen::Vector3f surfNormal = Eigen::Vector3f::Zero();
		Eigen::Vector3f shadNormal = Eigen::Vector3f::Zero();
		Eigen::Vector3f U_vector = Eigen::Vector3f::Zero();
		Eigen::Vector2f UvCoord = Eigen::Vector2f::Zero();
		float U_factor = 0.f, V_factor = 0.f, Distance = 0.f;
		uint32_t primitiveIndex = 0;

		void resetStruct()
		{
			obj = nullptr;
			Material = nullptr;
			Point = surfNormal = shadNormal = U_vector = Eigen::Vector3f::Zero();
			UvCoord = Eigen::Vector2f::Zero();
			U_factor = V_factor = Distance = 0.0f;
		}
	};

	class Object
	{
	protected:
		std::shared_ptr<Material> material{};

		std::vector<Light*> primitives_lights{};

		Eigen::AlignedBox3f Object_bounds{};
	public:
		std::string name{};

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Object()
		{
			
		}

		virtual ~Object();

		/**
		 * \brief Checks if a primitive in the object is hit by the incoming ray and save the hit information if it was hit.
		 * \param incoming_ray 
		 * \param primitive_index 
		 * \param hit_info Will be updated with the hit information if there is a hit with the primitive.
		 * \return The distance between the ray origin and the hit point, or -1 if there is no hit.
		 */
		virtual float PrimitiveHitByRay(const Ray& incoming_ray, int primitive_index, HitInfo& hit_info) const = 0;

		/**
		 * \brief Checks if a primitive in the object is hit by the incoming ray.
		 * \param incoming_ray 
		 * \param primitive_index 
		 * \return The distance between the ray origin and the hit point, or -1 if there is no hit.
		 */
		virtual float PrimitiveHitByRay(const Ray& incoming_ray, int primitive_index) const = 0;

		void SetMaterial(std::shared_ptr<Material> material);
		Material* GetMaterial() const;

		void SetPrimitiveLight(uint32_t primitiveIndex, Light* light);
		Light* GetPrimitiveLight(uint32_t primitiveIndex) const;

		// Set the whole object bounds
		void SetBounds(const Eigen::AlignedBox3f& bounds);
		// Get the whole object bounds
		const Eigen::AlignedBox3f& GetBounds() const { return this->Object_bounds; }

		/**
		 * \brief Returns the bounds of every primitive contained in the object (e.g. each triangle bound in a mesh).
		 * \return The vector containing the bounds of each primitive, ordered by primitive index.
		 */
		virtual std::vector<Eigen::AlignedBox3f> GetPrimitivesBounds() const = 0;

		/**
		 * \brief 
		 * \return The number of primitives contained in this object
		 */
		virtual uint32_t GetPrimitiveCount() const = 0;


		virtual HitInfo SamplePrimitivePoint(uint32_t primitiveIndex) const = 0;
		virtual float PrimitiveSamplePDF(uint32_t primitiveIndex) const = 0;
	};
}

#endif