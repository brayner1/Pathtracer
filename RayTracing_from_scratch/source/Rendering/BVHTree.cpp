#include "pch.h"
#include "Rendering/BVHTree.h"
#include "RenderHeaders.h"

using namespace Renderer;

BVHTree::BVHTree(std::vector<Object*>& objects) : Objects(objects)
{
	heuristic = SplitHeuristic::MiddleSplit;
	int ObjCount = Objects.size();

	std::vector<ObjectBVHInfo> objectsInfo(ObjCount);
	for (int i = 0; i < ObjCount; i++)
	{
		objectsInfo[i] = ObjectBVHInfo(i, Objects[i]->GetBounds());
	}

	int totalNodes = 0;
	
	std::vector<Object*> OrderedObjs;
	OrderedObjs.reserve(ObjCount);

	BVHTreeNode root;
	NodeArray.push_back(root);

	root = RecursiveBuildTree(OrderedObjs, objectsInfo, 0, ObjCount, totalNodes);
	NodeArray[0] = root;
	std::cout << "BVH root second child: " << std::endl;
	std::cout << root.SecondChildOffset << std::endl;

	Objects.swap(OrderedObjs);

	std::cout << Objects[0]->GetBounds().min() << std::endl;
}

BVHTreeNode BVHTree::RecursiveBuildTree(std::vector<Object*>& orderedObjs, std::vector<ObjectBVHInfo>& objsInfo, int start, int end, int& totalNodes)
{
	int nObjs = end - start;
	BVHTreeNode node;

	// Calculate node bound
	Eigen::AlignedBox3f bound = objsInfo[start].bounds;
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
			int objIndex = objsInfo[i].index;
			orderedObjs.push_back(Objects[objIndex]);
		}

		Eigen::AlignedBox3f objBound = orderedObjs[firstObjIndex]->GetBounds();
		std::cout << "Object min bound: " << objBound.min().transpose() << std::endl;
		std::cout << "node min bound: " << bound.min().transpose() << std::endl;

		std::cout << "Object max bound: " << objBound.max().transpose() << std::endl;
		std::cout << "node max bound: " << bound.max().transpose() << std::endl;

		node.Leaf(firstObjIndex, nObjs, bound);
		//NodeArray.push_back(node);

		return node;
	}
	
	// Calculate Centroid Bounds
	Eigen::AlignedBox3f centroidBounds;
	for (int i = 0; i < end; i++)
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
			int objIndex = objsInfo[i].index;
			orderedObjs.push_back(Objects[objIndex]);
		}

		node.Leaf(firstObjIndex, nObjs, bound);
		//NodeArray.push_back(node);

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
				if (j == NSlices) j--;

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
			for (int i = 0; i < NSlices - 1; i++)
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

				std::cout << "adding same SAH non dividible primitives: " << nObjs << std::endl;

				for (int i = start; i < end; i++)
				{
					int objIndex = objsInfo[i].index;
					orderedObjs.push_back(Objects[objIndex]);
				}

				node.Leaf(firstObjIndex, nObjs, bound);
				//NodeArray.push_back(node);

				return node;
			}


		}
		break;
	}
	}

	int nodeIndex = NodeArray.size() - 1;

	std::cout << "creating node " << nodeIndex << std::endl;
	//NodeArray.push_back(node);

	std::cout << "selected dimension: " << dim << std::endl;

	std::cout << "left child index: " << NodeArray.size() << std::endl;
	int leftIndex = NodeArray.size();
	BVHTreeNode left;
	NodeArray.push_back(left);
	left = RecursiveBuildTree(orderedObjs, objsInfo, start, mid, totalNodes);
	NodeArray[leftIndex] = left;
	//left = RecursiveBuildTree(orderedObjs, objsInfo, start, mid, totalNodes);
	BVHTreeNode right;
	int SecondChildIndex = NodeArray.size();
	std::cout << "node " << nodeIndex << " second child index: " << SecondChildIndex << std::endl;
	NodeArray.push_back(right);
	right	= RecursiveBuildTree(orderedObjs, objsInfo, mid, end, totalNodes);
	NodeArray[SecondChildIndex] = right;

	Eigen::AlignedBox3f bounds = NodeArray[leftIndex].Bounds.merged(NodeArray[SecondChildIndex].Bounds);

	node.Intermediate(dim, SecondChildIndex, bound);
	std::cout << "node " << nodeIndex << " second child index: " << node.SecondChildOffset << std::endl;

	//node->Intermediate()

	/*node->Intermediate(dim,
		RecursiveBuildTree(orderedObjs, objsInfo, start, mid, totalNodes),
		RecursiveBuildTree(orderedObjs, objsInfo, mid, end, totalNodes));*/



	return node;
}

bool Renderer::BVHTree::Intersect(Ray& ray, HitInfo& hit)
{
	bool has_hit = false;
	Eigen::Vector4f Dir = ray.getDirection();
	bool DirIsNeg[3] = { Dir(0) < 0.0f, Dir(1) < 0.0f, Dir(2) < 0.0f };
	Eigen::Vector4f invDir = Eigen::Vector4f(1.0f / Dir.x(), 1.0f / Dir.y(), 1.0f / Dir.z(), 0.0f);//Dir.cwiseInverse();
	int currentNode = 0;
	float min_dist = FLT_MAX;

	std::vector<int> VisitStack;
	VisitStack.reserve(128);

	//std::cout << "start bvh intersection " << VisitStack.size()  << std::endl;

	while (true)
	{
		//std::cout << "infinite loop? : " << hit.x << ", " << hit.y << std::endl;
		BVHTreeNode* node = &NodeArray[currentNode];

		if (BoundingBoxIntersect(ray, invDir, node->Bounds))
		{
			
			if (node->NumObjs > 0)
			{
				//std::cout << "Leaf bound hit" << std::endl;
				int objOffset = node->ObjOffset, finalOffset = node->ObjOffset + node->NumObjs;
				//std::cout << "currentNode: " << currentNode << std::endl;
				for (int i = objOffset; i < finalOffset; i++)
				{
					if (!Objects[i])
						std::cout << "testing obj: " << i << std::endl;
					HitInfo hit_info = hit;
					if (Objects[i]->is_hit_by_ray(ray, hit_info))
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
				if (DirIsNeg[node->SplitAxis])
				{
					VisitStack.push_back(currentNode + 1);
					currentNode = node->SecondChildOffset;
				}
				else
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

bool Renderer::BVHTree::Intersect(Ray& ray)
{
	//bool has_hit = false;
	Eigen::Vector4f Dir = ray.getDirection();
	bool DirIsNeg[3] = { Dir.x() < 0.0f, Dir.y() < 0.0f, Dir.z() < 0.0f };
	Eigen::Vector4f invDir = Eigen::Vector4f(1.0f / Dir.x(), 1.0f / Dir.y(), 1.0f / Dir.z(), 0.0f);
	int currentNode = 0;
	//float min_dist = FLT_MAX;

	std::vector<int> VisitStack;
	VisitStack.reserve(128);

	while (true)
	{
		BVHTreeNode* node = &NodeArray[currentNode];
		
		if (BoundingBoxIntersect(ray, invDir, node->Bounds))
		{
			if (node->NumObjs > 0)
			{
				int objOffset = node->ObjOffset, finalOffset = node->ObjOffset + node->NumObjs;
				for (int i = objOffset; i < finalOffset; i++)
				{
					HitInfo hit_info;
					if (Objects[i]->is_hit_by_ShadowRay(ray))
					{
						//has_hit = true;
						//std::cout << "has shadow" << std::endl;
						return true;
					}
				}

				if (VisitStack.size() == 0) break;
				currentNode = VisitStack.back();
				VisitStack.pop_back();

			}
			else
			{
				if (DirIsNeg[node->SplitAxis])
				{
					VisitStack.push_back(currentNode + 1);
					currentNode = node->SecondChildOffset;
				}
				else
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
	//std::cout << "no shadow" << std::endl;
	return false;
}


void Renderer::BVHTree::PrinTree()
{
	int count = 0, currentNode = 0;

	std::vector<int> VisitStack;
	VisitStack.reserve(128);

	std::cout << "node0 right child: " << NodeArray[0].SecondChildOffset << std::endl;

	std::cout << "node count: " << NodeArray.size() << std::endl;
	std::cout << "Object Count: " << Objects.size() << std::endl;

	while (true)
	{
		BVHTreeNode* node = &NodeArray[currentNode];

		std::cout << "node " << currentNode << std::endl;
		std::cout << "nObjects: " << node->NumObjs << std::endl;
		std::cout << "bounds:" << std::endl;
		std::cout << "	Min: " << node->Bounds.min().transpose() << std::endl;
		std::cout << "	Max: " << node->Bounds.max().transpose() << std::endl;

		if (node->NumObjs > 0)
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