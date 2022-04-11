#ifndef PTracer_BVHTree_h
#define PTracer_BVHTree_h
#include "Rendering/NodeHierarchy.h"

namespace Renderer
{

	struct ObjectBVHInfo
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		ObjectBVHInfo() {}
		ObjectBVHInfo(int objectIdx, int primIdx, const Eigen::AlignedBox3f& b) : objectIndex(objectIdx), primitiveIndex(primIdx), bounds(b), centroid(b.center()) {}

		int objectIndex;
		int primitiveIndex;
		Eigen::AlignedBox3f bounds;
		Eigen::Vector3f centroid;
	};

	struct BVHTreeNode
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		void Leaf(int primIndex, int numPrims, const Eigen::AlignedBox3f& b)
		{
			primOffset = primIndex;
			NumPrimitives = numPrims;
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
			int primOffset;
		};
		uint16_t NumPrimitives;
		uint8_t SplitAxis;
		uint8_t pad;	// 1 byte pad
	};

	

	class BVHTree :
		public NodeHierarchy
	{
	public:

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		enum class SplitHeuristic { SurfaceArea, MiddleSplit, EqualCount };

		BVHTree(const std::vector<Object*>& objects);

		bool Intersect(const Ray& ray, HitInfo& hit) override;
		float Intersect(const Ray& ray) override;

		void PrintTree();
		void PrintTree(std::string file_path);

	private:
		SplitHeuristic heuristic = SplitHeuristic::SurfaceArea;
		//std::vector<Object*> objects;
		std::vector<NodePrimitive> primitives;
		std::vector<BVHTreeNode> nodeArray;

		void LogPrimitivesInfo(const std::vector<ObjectBVHInfo>& primsInfo, std::string path);

		BVHTreeNode RecursiveBuildTree(std::vector<NodePrimitive>& orderedPrimitives, std::vector<ObjectBVHInfo>& objsInfo, int start, int end, int& totalNodes);
	};

}

#endif