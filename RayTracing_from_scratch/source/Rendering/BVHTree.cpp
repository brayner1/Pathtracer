#include "pch.h"
#include "Rendering/BVHTree.h"
#include "RenderHeaders.h"

using namespace Renderer;

BVHTree::BVHTree(std::vector<Object*>& objects) : objects(objects)
{
	int ObjCount = objects.size();

	std::vector<ObjectBVHInfo> objectsInfo(ObjCount);
	for (int i = 0; i < ObjCount; i++)
	{
		objectsInfo[i] = ObjectBVHInfo(i, objects[i]->GetBounds());
	}

	int totalNodes = 0;
	
	std::vector<Object*> OrderedObjs;
	OrderedObjs.reserve(ObjCount);

	BVHTreeNode root;
	nodeArray.push_back(root);

	root = RecursiveBuildTree(OrderedObjs, objectsInfo, 0, ObjCount, totalNodes);
	nodeArray[0] = root;
	std::cout << "BVH root second child: " << std::endl;
	std::cout << root.SecondChildOffset << std::endl;

	this->objects.swap(OrderedObjs);

	std::cout << objects[0]->GetBounds().min() << std::endl;
}

BVHTreeNode BVHTree::RecursiveBuildTree(std::vector<Object*>& orderedObjs, std::vector<ObjectBVHInfo>& objsInfo, int start, int end, int& totalNodes)
{
	int nObjs = end - start;
	BVHTreeNode node;

	// Calculate node bound
	Eigen::AlignedBox3f bound;
	for (size_t i = start; i < end; i++)
	{
		bound.extend(objsInfo[i].bounds);
	}

	// Create Leaf Node if only one object left
	if (nObjs == 1)
	{

		std::cout << "adding single primitive" << std::endl;

		int firstObjIndex = orderedObjs.size();

		for (int i = start; i < end; i++)
		{
			int objIndex = objsInfo[i].objectIndex;
			orderedObjs.push_back(objects[objIndex]);
		}

		Eigen::AlignedBox3f objBound = orderedObjs[firstObjIndex]->GetBounds();
		std::cout << "Object min bound: " << objBound.min().transpose() << std::endl;
		std::cout << "node min bound: " << bound.min().transpose() << std::endl;

		std::cout << "Object max bound: " << objBound.max().transpose() << std::endl;
		std::cout << "node max bound: " << bound.max().transpose() << std::endl;

		node.Leaf(firstObjIndex, nObjs, bound);
		//nodeArray.push_back(node);

		return node;
	}
	
	// Calculate Centroid Bounds
	Eigen::AlignedBox3f centroidBounds;
	for (int i = start; i < end; i++)
	{
		Eigen::Vector3f centroid = objsInfo[i].centroid;
		centroidBounds.extend(centroid);
	}

	// Get the maximum extent dimension
	Eigen::Index dim;
	centroidBounds.diagonal().maxCoeff(&dim);

	// If objects centroids are the same, create Leaf Node
	if (centroidBounds.min()(dim) == centroidBounds.max()(dim))
	{
		int firstObjIndex = orderedObjs.size();

		std::cout << "adding same centroid primitives: " << nObjs << std::endl;

		for (int i = start; i < end; i++)
		{
			int objIndex = objsInfo[i].objectIndex;
			orderedObjs.push_back(objects[objIndex]);
		}

		node.Leaf(firstObjIndex, nObjs, bound);
		//nodeArray.push_back(node);

		return node;
	}

	int mid = (start + end) / 2;
	switch (heuristic)
	{
	case Renderer::BVHTree::SplitHeuristic::MiddleSplit:
	{

		float midPoint = (centroidBounds.min()(dim) + centroidBounds.max()(dim)) / 2.0f;

		// std::partition divides de array in two, putting in the left elements that fulfill the condition in the function passed.
		// it returns a pointer to the rightmost part of the array.
		ObjectBVHInfo* midPtr =
			std::partition(&objsInfo[start], &objsInfo[end - 1] + 1,
				[dim, midPoint](const ObjectBVHInfo& pi) {
					return pi.centroid(dim) < midPoint;
				});

		mid = midPtr - &objsInfo[0];
		if (mid != start && mid != end)
			break;
	}
	case Renderer::BVHTree::SplitHeuristic::EqualCount:
	{

		mid = (start + end) / 2;
		
		// nth element puts on the nth element position passed the value that would be there if the array were completely ordered.
		// To the left of the element is the elements that returned true to the condition, and to the right otherwise.
		std::nth_element(&objsInfo[start], &objsInfo[mid], &objsInfo[end - 1] + 1,
			[dim](const ObjectBVHInfo& a, const ObjectBVHInfo& b) { return a.centroid(dim) < b.centroid(dim); }
		);
		break;
	}
	case Renderer::BVHTree::SplitHeuristic::SurfaceArea:
	default:
	{
		if (nObjs == 2)
		{
			mid = (start + end) / 2;

			// nth element puts on the nth element position passed the value that would be there if the array were completely ordered.
			// To the left of the element is the elements that returned true to the condition, and to the right otherwise.
			std::nth_element(&objsInfo[start], &objsInfo[mid], &objsInfo[end - 1] + 1,
				[dim](const ObjectBVHInfo& a, const ObjectBVHInfo& b) { return a.centroid(dim) < b.centroid(dim); }
			);
		}
		else
		{
			const int NSlices = 32;
			struct Slice {
				int ObjCount = 0;
				Eigen::AlignedBox3f bounds;
			};
			Slice slices[NSlices];

			for (int i = start; i < end; i++)
			{
				int j = NSlices * BoundOffset(centroidBounds, objsInfo[i].centroid)(dim);
				if (j == NSlices) j -= 1;

				slices[j].ObjCount++;
				slices[j].bounds.extend(objsInfo[i].bounds);
			}
			
			float cost[NSlices - 1];

			for (int i = 0; i < NSlices - 1; i++)
			{
				Eigen::AlignedBox3f LeftBound, RightBound;
				int LeftCount = 0, RightCount = 0;

				for (int j = 0; j <= i; j++)
				{
					LeftBound.extend(slices[j].bounds);
					LeftCount += slices[j].ObjCount;
				}

				for (int j = i+1; j < NSlices; j++)
				{
					RightBound.extend(slices[j].bounds);
					RightCount += slices[j].ObjCount;
				}

				cost[i] = 0.125f + (LeftCount * BoundingBoxSurfaceArea(LeftBound) + RightCount * BoundingBoxSurfaceArea(RightBound)) / BoundingBoxSurfaceArea(bound);

			}

			float minCost = cost[0];
			int minCostIndex = 0;
			for (int i = 1; i < NSlices - 1; i++)
			{
				if (cost[i] < minCost)
				{
					minCost = cost[i];
					minCostIndex = i;
				}
			}

			float leafCost = nObjs;
			if (nObjs > 255 || minCost < leafCost)
			{
				// std::partition divides de array in two, putting in the left elements that fulfill the condition in the function passed.
				// it returns a pointer to the rightmost part of the array.
				ObjectBVHInfo* midPtr =
					std::partition(&objsInfo[start], &objsInfo[end - 1] + 1,
						[=](const ObjectBVHInfo& pi) {
							int j = NSlices * BoundOffset(centroidBounds, pi.centroid)(dim);
							if (j == NSlices) j--;
							return j <= minCostIndex;
						});

				mid = midPtr - &objsInfo[0];
			}
			else
			{
				int firstObjIndex = orderedObjs.size();

				std::cout << "adding same SAH non divisible primitives: " << nObjs << std::endl;

				for (int i = start; i < end; i++)
				{
					int objIndex = objsInfo[i].objectIndex;
					orderedObjs.push_back(objects[objIndex]);
				}

				node.Leaf(firstObjIndex, nObjs, bound);

				return node;
			}


		}
		break;
	}
	}

	int nodeIndex = nodeArray.size() - 1;

	std::cout << "creating node " << nodeIndex << std::endl;
	//nodeArray.push_back(node);

	std::cout << "selected dimension: " << dim << std::endl;

	std::cout << "left child objectIndex: " << nodeArray.size() << std::endl;
	int leftIndex = nodeArray.size();
	BVHTreeNode left;
	nodeArray.push_back(left);
	left = RecursiveBuildTree(orderedObjs, objsInfo, start, mid, totalNodes);
	nodeArray[leftIndex] = left;
	
	BVHTreeNode right;
	int SecondChildIndex = nodeArray.size();
	std::cout << "node " << nodeIndex << " second child objectIndex: " << SecondChildIndex << std::endl;
	nodeArray.push_back(right);
	right	= RecursiveBuildTree(orderedObjs, objsInfo, mid, end, totalNodes);
	nodeArray[SecondChildIndex] = right;

	Eigen::AlignedBox3f bounds = nodeArray[leftIndex].Bounds.merged(nodeArray[SecondChildIndex].Bounds);

	node.Intermediate(dim, SecondChildIndex, bound);
	std::cout << "node " << nodeIndex << " second child objectIndex: " << node.SecondChildOffset << std::endl;

	//node->Intermediate()

	/*node->Intermediate(dim,
		RecursiveBuildTree(orderedObjs, objsInfo, start, mid, totalNodes),
		RecursiveBuildTree(orderedObjs, objsInfo, mid, end, totalNodes));*/



	return node;
}

bool Renderer::BVHTree::Intersect(const Ray& ray, HitInfo& hit)
{
	bool has_hit = false;
	Eigen::Vector3f Dir = ray.getDirection();
	Eigen::Vector3f invDir = Eigen::Vector3f(1.0f / Dir.x(), 1.0f / Dir.y(), 1.0f / Dir.z());//Dir.cwiseInverse();
	//bool DirIsNeg[3] = { Dir.x() < 0.0f, Dir.y() < 0.0f, Dir.z() < 0.0f };
	int currentNode = 0;
	float min_dist = FLT_MAX;

	std::vector<int> VisitStack;
	VisitStack.reserve(128);

	//std::cout << "start bvh intersection " << VisitStack.size()  << std::endl;

	while (true)
	{
		//std::cout << "infinite loop? : " << hit.x << ", " << hit.y << std::endl;
		BVHTreeNode* node = &nodeArray[currentNode];

		if (BoundingBoxIntersect(ray, invDir, node->Bounds))
		{
			
			if (node->NumPrimitives > 0)
			{
				//std::cout << "Leaf bound hit" << std::endl;
				const int objOffset = node->pimOffset, finalOffset = node->pimOffset + node->NumPrimitives;
				//std::cout << "currentNode: " << currentNode << std::endl;
				for (int i = objOffset; i < finalOffset; i++)
				{
					HitInfo hit_info = hit;
					if (objects[i]->isHitByRay(ray, hit_info))
					{
						if (hit_info.Distance <= min_dist) {
							has_hit = true;
							hit = hit_info;
							min_dist = hit_info.Distance;
						}
					}
				}
				if (VisitStack.size() == 0) break;
				currentNode = VisitStack.back();
				VisitStack.pop_back();
				
		 	}
			else
			{
				/*if (DirIsNeg[node->SplitAxis])
				{
					VisitStack.push_back(currentNode + 1);
					currentNode = node->SecondChildOffset;
				}
				else*/
				{
					currentNode++;
					VisitStack.push_back(node->SecondChildOffset);
				}
			}

		}
		else
		{
			if (VisitStack.size() == 0) break;
			currentNode = VisitStack.back();
			VisitStack.pop_back();
		}


	}
	//if (hit.x == 256 && hit.y == 256)
		//std::cout << "hey it has and end" << std::endl;
	/*if (has_hit)
		std::cout << "hit" << std::endl;*/

	return has_hit;
}

float Renderer::BVHTree::Intersect(const Ray& ray)
{
	//bool has_hit = false;
	Eigen::Vector3f Dir = ray.getDirection();
	Eigen::Vector3f invDir = Dir.cwiseInverse();
	Eigen::Vector3i DirIsNeg = { Dir.x() < 0, Dir.y() < 0, Dir.z() < 0 };
	int visitOffset = 0, currentNode = 0;
	float min_dist = FLT_MAX;
	bool hasHit = false;

	std::vector<int> VisitStack;
	VisitStack.reserve(128);

	while (true)
	{
		BVHTreeNode* node = &nodeArray[currentNode];

		if (BoundingBoxIntersect(ray, invDir, node->Bounds))
		{
			if (node->NumPrimitives > 0)
			{
				const int objOffset = node->pimOffset, finalOffset = node->pimOffset + node->NumPrimitives;
				for (int i = objOffset; i < finalOffset; i++)
				{
					float t = objects[i]->isHitByRay(ray);
					if (t > 0.f && t < min_dist)
					{
						hasHit = true;
						min_dist = t;
					}
				}
				if (VisitStack.size() == 0) break;
				currentNode = VisitStack.back();
				VisitStack.pop_back();
			}
			else
			{
				/*if (DirIsNeg[node->SplitAxis])
				{
					VisitStack.push_back(currentNode + 1);
					currentNode = node->SecondChildOffset;
				}
				else*/
				{
					currentNode++;
					VisitStack.push_back(node->SecondChildOffset);
				}
			}

		}
		else
		{
			if (VisitStack.size() == 0) break;
			currentNode = VisitStack.back();
			VisitStack.pop_back();
		}


	}

	return hasHit? min_dist : -1.f;
}


void Renderer::BVHTree::PrintTree()
{
	int count = 0, currentNode = 0;

	std::vector<int> VisitStack;
	VisitStack.reserve(128);

	std::cout << "node0 right child: " << nodeArray[0].SecondChildOffset << std::endl;

	std::cout << "node count: " << nodeArray.size() << std::endl;
	std::cout << "Object Count: " << objects.size() << std::endl;

	while (true)
	{
		BVHTreeNode* node = &nodeArray[currentNode];

		std::cout << "node " << currentNode << std::endl;
		std::cout << "nObjects: " << node->NumPrimitives << std::endl;
		std::cout << "bounds:" << std::endl;
		std::cout << "	Min: " << node->Bounds.min().transpose() << std::endl;
		std::cout << "	Max: " << node->Bounds.max().transpose() << std::endl;

		if (node->NumPrimitives > 0)
		{
			std::cout << "Leaft Node;" << std::endl << std::endl;
			if (VisitStack.size() == 0) break;
			currentNode = VisitStack.back();
			VisitStack.pop_back();
		}
		else
		{
			std::cout << "left child: " << currentNode + 1 << "; right child: " << node->SecondChildOffset << std::endl << std::endl;
			currentNode = currentNode + 1;
			VisitStack.push_back(node->SecondChildOffset);
		}

		//if (VisitStack.size() == 0) break;

	}
	return;
}