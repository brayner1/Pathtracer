#pragma once
#include "Rendering/Ray.h"
#include "Object/Object.h"

namespace Renderer
{


	class ObjectTree
	{
	public:

		virtual bool Intersect(Ray& ray) = 0;
		virtual bool Intersect(Ray& ray, HitInfo& hit) = 0;

	};

}
