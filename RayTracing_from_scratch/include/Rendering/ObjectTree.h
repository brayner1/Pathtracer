#pragma once
#include "Rendering/Ray.h"
#include "Object/Object.h"

namespace Renderer
{


	class ObjectTree
	{
	public:
		virtual ~ObjectTree() = default;

		virtual bool Intersect(const Ray& ray, HitInfo& hit) = 0;
		virtual float Intersect(const Ray& ray) = 0;

	};

}
