#pragma once
#include "Rendering/ObjectTree.h"

namespace Renderer
{

	struct ObjectBVHInfo
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		ObjectBVHInfo() {}
		ObjectBVHInfo(int i, Eigen::AlignedBox3f& b) : index(i), bounds(b), centroid(b.center()) {}

		int index;
		Eigen::AlignedBox3f bounds;
		Eigen::Vector3f centroid;
	};

	struct BVHTreeNode
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		void Leaf(int objIndex, int numObjs, const Eigen::AlignedBox3f& b)
		{
			ObjOffset = objIndex;
			NumObjs = numObjs;
			Bounds = b;

			SplitAxis = -1;

			//child[0] = child[1] = nullptr;
		}

		//void Intermediate(int axis, BVHTreeNode* left, BVHTreeNode* right)
		void Intermediate(int axis, int Child2Index, Eigen::AlignedBox3f& b)
		{
			SplitAxis = axis;
			SecondChildOffset = Child2Index;
			Bounds = b;
			
			//left->bounds.merged(right->bounds);
			/*child[0] = left;
			child[1] = right;*/

			NumObjs = 0;
		}

		Eigen::AlignedBox3f Bounds;
		//BVHTreeNode* child[2];

		union
		{
			int SecondChildOffset;
			int ObjOffset;
		};
			uint16_t NumObjs;
			uint8_t SplitAxis;
			uint8_t pad;	// 1 byte pad
	};

	class BVHTree :
		public ObjectTree
	{
	public:

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		enum class SplitHeuristic { SurfaceArea, MiddleSplit, EqualCount };

		BVHTree(std::vector<Object*>& objects);

		bool Intersect(const Ray& ray, HitInfo& hit) override;
		float Intersect(const Ray& ray) override;

		void PrintTree();


	private:
		SplitHeuristic heuristic = SplitHeuristic::SurfaceArea;
		std::vector<Object*> Objects;
		std::vector<BVHTreeNode> NodeArray;

		BVHTreeNode RecursiveBuildTree(std::vector<Object*>& orderedObjs, std::vector<ObjectBVHInfo>& objsInfo, int start, int end, int& totalNodes);
	};

}
