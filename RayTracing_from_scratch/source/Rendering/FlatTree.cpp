#include "pch.h"
#include "Rendering\FlatTree.h"
#include "RenderHeaders.h"

using namespace Renderer;

bool FlatTree::Intersect(const Ray& ray, HitInfo& hit)
{
	Eigen::Vector3f Dir = ray.getDirection();
	Eigen::Vector3f invDir = Eigen::Vector3f(1.0f / Dir.x(), 1.0f / Dir.y(), 1.0f / Dir.z());

	float min_dist = std::numeric_limits<float>::max();
	bool has_hit = false;

	for (int i = 0; i < Objects.size(); i++) {
		HitInfo hit_info = hit;
		Eigen::AlignedBox3f bounds = Objects[i]->GetBounds();
		if (BoundingBoxIntersect(ray, invDir, bounds))
			if (Objects[i]->isHitByRay(ray, hit_info)) {
				if (hit_info.Distance <= min_dist) {
					has_hit = true;
					hit = hit_info;
					min_dist = hit_info.Distance;
				}
			}
	}
	return has_hit;
}

float FlatTree::Intersect(const Ray& ray)
{
	float min_dist = std::numeric_limits<float>::max();
	for (int i = 0; i < Objects.size(); i++) {
		float t = Objects[i]->isHitByRay(ray);
		if (t > 0.f && t < min_dist) {
			min_dist = t;
		}
	}
	return min_dist != std::numeric_limits<float>::max()? min_dist : -1.f;
}
