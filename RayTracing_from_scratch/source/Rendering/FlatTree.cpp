#include "pch.h"
#include "Rendering\FlatTree.h"
#include "RenderHeaders.h"

using namespace Renderer;

bool FlatTree::Intersect(Ray& ray, HitInfo& hit)
{
	Eigen::Vector4f Dir = ray.getDirection();
	Eigen::Vector4f invDir = Eigen::Vector4f(1.0f / Dir.x(), 1.0f / Dir.y(), 1.0f / Dir.z(), 0.0f);

	float min_dist = std::numeric_limits<float>::max();
	bool has_hit = false;

	for (int i = 0; i < Objects.size(); i++) {
		HitInfo hit_info = hit;
		Eigen::AlignedBox3f bounds = Objects[i]->GetBounds();
		if (BoundingBoxIntersect(ray, invDir, bounds))
			if (Objects[i]->is_hit_by_ray(ray, hit_info)) {
				if (hit_info.Distance <= min_dist) {
					has_hit = true;
					hit = hit_info;
					min_dist = hit_info.Distance;
				}
			}
	}
	return has_hit;
}

bool FlatTree::Intersect(Ray& ray)
{
	for (int i = 0; i < Objects.size(); i++) {
		HitInfo temp;
		if (Objects[i]->is_hit_by_ray(ray, temp)) {
			return true;
		}
	}
	return false;
}
