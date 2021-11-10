#pragma once
#include "Rendering/ObjectTree.h"

namespace Renderer
{

	struct ObjectBVHInfo
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		ObjectBVHInfo() {}
		ObjectBVHInfo(int i, const Eigen::AlignedBox3f& b) : objectIndex(i), bounds(b), centroid(b.center()) {}

		int objectIndex;
		int primitiveIndex;
		Eigen::AlignedBox3f bounds;
		Eigen::Vector3f centroid;
	};

	struct BVHTreeNode
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		void Leaf(int objIndex, int numObjs, const Eigen::AlignedBox3f& b)
		{
			pimOffset = objIndex;
			NumPrimitives = numObjs;
			Bounds = b;

			SplitAxis = -1;
		}

		//void Intermediate(int axis, BVHTreeNode* left, BVHTreeNode* right)
		void Intermediate(int axis, int Child2Index, Eigen::AlignedBox3f& b)
		{
			SplitAxis = axis;
			SecondChildOffset = Child2Index;
			Bounds = b;

			NumPrimitives = 0;
		}

		Eigen::AlignedBox3f Bounds;

		union
		{
			int SecondChildOffset;
			int pimOffset;
		};
		uint16_t NumPrimitives;
		uint8_t SplitAxis;
		uint8_t pad;	// 1 byte pad
	};

	struct NodePrimitive
	{
		int objectIndex;
		int primitiveIndex;
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
		std::vector<Object*> objects;
		std::vector<NodePrimitive> primitives;
		std::vector<BVHTreeNode> nodeArray;

		BVHTreeNode RecursiveBuildTree(std::vector<Object*>& orderedObjs, std::vector<ObjectBVHInfo>& objsInfo, int start, int end, int& totalNodes);
	};

}
