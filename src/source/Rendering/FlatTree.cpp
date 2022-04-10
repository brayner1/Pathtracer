#include "pch.h"
#include "Rendering\FlatTree.h"
#include "RenderHeaders.h"

using namespace Renderer;

FlatTree::FlatTree(std::vector<Object*>& objects)
{
	this->objects = objects;
	for (uint32_t i = 0; i < objects.size(); ++i)
	{
		uint32_t primitiveCount = objects[i]->GetPrimitiveCount();
		for (uint32_t k = 0; k < primitiveCount; ++k)
		{
			NodePrimitive objectNode {i, k};
			primitives.emplace_back(objectNode);
			
		}
	}
}

bool FlatTree::Intersect(const Ray& ray, HitInfo& hit_info)
{
	Eigen::Vector3f Dir = ray.getDirection();
	Eigen::Vector3f invDir = Eigen::Vector3f(1.0f / Dir.x(), 1.0f / Dir.y(), 1.0f / Dir.z());

	float min_dist = std::numeric_limits<float>::max();
	bool has_hit = false;

	for (const auto& [objectIndex, primitiveIndex] : primitives)
	{
		const Object* object = objects[objectIndex];
		HitInfo tmpHit = hit_info;
		const float t = object->PrimitiveHitByRay(ray, primitiveIndex, tmpHit);
		if (t > 0.f && t < min_dist)
		{
			has_hit = true;
			hit_info = tmpHit;
			min_dist = t;
		}
	}

	return has_hit;
}

float FlatTree::Intersect(const Ray& ray)
{
	float min_dist = std::numeric_limits<float>::max();
	for (auto [objectIndex, primitiveIndex] : primitives)
	{
		const Object* object = objects[objectIndex];
		const float t = object->PrimitiveHitByRay(ray, primitiveIndex);
		if (t > 0.f && t < min_dist)
		{
			min_dist = t;
		}
	}
	return min_dist != std::numeric_limits<float>::max()? min_dist : -1.f;
}
