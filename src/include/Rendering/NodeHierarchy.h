#pragma once
#include "Rendering/Ray.h"
#include "Object/Object.h"

namespace Renderer
{
	struct NodePrimitive
	{
		int objectIndex;
		int primitiveIndex;
	};

	class NodeHierarchy
	{
	public:
		virtual ~NodeHierarchy() = default;

		virtual bool Intersect(const Ray& ray, HitInfo& hit) = 0;
		virtual float Intersect(const Ray& ray) = 0;

	protected:
		std::vector<Object*> objects;

	};
}
