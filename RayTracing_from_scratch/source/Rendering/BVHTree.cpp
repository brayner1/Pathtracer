#include "pch.h"
#include "Rendering/BVHTree.h"
#include "RenderHeaders.h"

using namespace Renderer;

//#define LOAD_DEBUG
#ifdef LOAD_DEBUG
static std::ofstream buildLog;
#endif

BVHTree::BVHTree(const std::vector<Object*>& objects) : objects(objects)
{
	int ObjCount = objects.size();

	std::vector<ObjectBVHInfo> objectsInfo;//(ObjCount);
	for (int i = 0; i < ObjCount; i++)
	{
		//objectsInfo[i] = ObjectBVHInfo(i, objects[i]->GetBounds());
		std::vector<Eigen::AlignedBox3f> objPrimitivesBounds = objects[i]->GetPrimitivesBounds();
		for (int prim = 0; prim < objPrimitivesBounds.size(); prim++)
		{
			objectsInfo.emplace_back(i, prim, objPrimitivesBounds[prim]);
		}
	}

	LogPrimitivesInfo(objectsInfo, "Log/PrimitivesInfo_BVH.txt");

	int totalNodes = 0;
	
	/*std::vector<Object*> OrderedObjs;
	OrderedObjs.reserve(ObjCount);*/

	std::vector<NodePrimitive> orderedPrimitives;
	orderedPrimitives.reserve(objectsInfo.size());

	BVHTreeNode root;
	nodeArray.push_back(root);
#ifdef LOAD_DEBUG
	buildLog.open("Log/TreeBuild.txt");
	std::cout << "opened build log file\n";
#endif
	root = RecursiveBuildTree(orderedPrimitives, objectsInfo, 0, objectsInfo.size(), totalNodes);
	nodeArray[0] = root;
#ifdef LOAD_DEBUG
	buildLog.close();
#endif
	//this->objects.swap(OrderedObjs);
	this->primitives.swap(orderedPrimitives);

	std::cout << objects[0]->GetBounds().min() << std::endl;
}

BVHTreeNode BVHTree::RecursiveBuildTree(std::vector<NodePrimitive>& orderedPrimitives, std::vector<ObjectBVHInfo>& objsInfo, int start, int end, int& totalNodes)
{
	int nObjs = end - start;
	BVHTreeNode node;
#ifdef LOAD_DEBUG
		buildLog << "Building node that contains " << nObjs << " primitives, starting from index " << start << " and ending at " << end << std::endl;
#endif

	// Calculate node bound
	Eigen::AlignedBox3f nodeBound{};
	for (size_t i = start; i < end; i++)
	{
		nodeBound.extend(objsInfo[i].bounds);
	}

#ifdef LOAD_DEBUG
		buildLog << "Node bounds:\n\tmin: " << nodeBound.min().transpose() << "\n\tmax: " << nodeBound.max().transpose() << std::endl;
#endif

	// Create Leaf Node if only one object left
	if (nObjs == 1)
	{
		int firstObjIndex = orderedPrimitives.size();

		for (int i = start; i < end; i++)
		{
			orderedPrimitives.push_back(NodePrimitive{objsInfo[i].objectIndex, objsInfo[i].primitiveIndex});
			//orderedPrimitives.push_back(objects[objIndex]);
		}

		#ifdef LOAD_DEBUG
		buildLog << "Node is a leaf with 1 primitive at index: " << firstObjIndex << std::endl;
		#endif

		node.Leaf(firstObjIndex, nObjs, nodeBound);

		return node;
	}
	
	// Calculate Centroid Bounds
	Eigen::AlignedBox3f centroidBounds;
	for (int i = start; i < end; i++)
	{
		Eigen::Vector3f centroid = objsInfo[i].centroid;
		centroidBounds.extend(centroid);
	}

#ifdef LOAD_DEBUG
		buildLog << "Node centroid bounds:\n\tmin: " << centroidBounds.min().transpose() << "\n\tmax: " << centroidBounds.max().transpose() << std::endl;
#endif

	// Get the maximum extent dimension
	Eigen::Index dim;
	centroidBounds.diagonal().maxCoeff(&dim);

#ifdef LOAD_DEBUG
		buildLog << "Node split dimension: " << dim << std::endl;
#endif

	// If objects centroids are the same, create Leaf Node
	if (centroidBounds.min()(dim) == centroidBounds.max()(dim))
	{
		int firstObjIndex = orderedPrimitives.size();
		
		for (int i = start; i < end; i++)
		{
			orderedPrimitives.push_back(NodePrimitive{objsInfo[i].objectIndex, objsInfo[i].primitiveIndex});
		}

		#ifdef LOAD_DEBUG
		buildLog << "Node centroid bound is a point. Creating leaf with " << nObjs << " primitives, starting at index " << firstObjIndex << std::endl;
		#endif

		node.Leaf(firstObjIndex, nObjs, nodeBound);
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

		#ifdef LOAD_DEBUG
			buildLog << "Node contains only 2 primitives, so it will be split in half at index " << mid << std::endl;
		#endif
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

				slices[j].ObjCount += 1;
				slices[j].bounds.extend(objsInfo[i].bounds);
			}

			#ifdef LOAD_DEBUG
			for (int i = 0; i < NSlices; ++i)
				{
					buildLog << "Slice["<<i<<"]:\n\tPrimitives: " << slices[i].ObjCount << "\n\tBounds:\n\t\tmin:" << 
						slices[i].bounds.min().transpose() << "\n\t\tmax: " <<  slices[i].bounds.max().transpose() << std::endl;
				}
			#endif
			
			float cost[NSlices - 1];

			#ifdef LOAD_DEBUG
				buildLog << "Slices cost: " << std::endl;
			#endif

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

				cost[i] = 0.125f + (LeftCount * BoundingBoxSurfaceArea(LeftBound) + RightCount * BoundingBoxSurfaceArea(RightBound)) / BoundingBoxSurfaceArea(nodeBound);
				#ifdef LOAD_DEBUG
				buildLog << "Slice["<<i<<"] cost: " << cost[i] << std::endl;
				#endif
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

			#ifdef LOAD_DEBUG
			buildLog << "Selected slice index is " << minCostIndex << " with cost " << minCost << std::endl;
			#endif

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

				#ifdef LOAD_DEBUG
				buildLog << "Dividing primitives at index " << mid << std::endl;
				#endif
			}
			else
			{
				int firstObjIndex = orderedPrimitives.size();
				
				for (int i = start; i < end; i++)
				{
					int objIndex = objsInfo[i].objectIndex;
					orderedPrimitives.push_back(NodePrimitive{objsInfo[i].objectIndex, objsInfo[i].primitiveIndex});
				}

				node.Leaf(firstObjIndex, nObjs, nodeBound);

				#ifdef LOAD_DEBUG
				buildLog << "Minimal cost slice is not worth. Creating leaf node with " << nObjs << 
						" primitives, starting at index " << firstObjIndex  << std::endl;
				#endif

				return node;
			}


		}
		break;
	}
	}

	int leftIndex = nodeArray.size();
	BVHTreeNode left;
	nodeArray.push_back(left);
	left = RecursiveBuildTree(orderedPrimitives, objsInfo, start, mid, totalNodes);
	nodeArray[leftIndex] = left;
	
	BVHTreeNode right;
	int SecondChildIndex = nodeArray.size();

	nodeArray.push_back(right);
	right = RecursiveBuildTree(orderedPrimitives, objsInfo, mid, end, totalNodes);
	nodeArray[SecondChildIndex] = right;

	Eigen::AlignedBox3f bounds = nodeArray[leftIndex].Bounds.merged(nodeArray[SecondChildIndex].Bounds);

	node.Intermediate(dim, SecondChildIndex, nodeBound);

	return node;
}

bool BVHTree::Intersect(const Ray& ray, HitInfo& hit)
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
				const int primOffset = node->primOffset, finalOffset = node->primOffset + node->NumPrimitives;
				//std::cout << "currentNode: " << currentNode << std::endl;
				for (int i = primOffset; i < finalOffset; i++)
				{
					HitInfo hit_info = hit;
					const Object* object = objects[primitives[i].objectIndex];
					//if (objects[i]->isHitByRay(ray, hit_info))
					if (object->isPrimitiveHitByRay(ray,  primitives[i].primitiveIndex, hit_info) > 0.f)
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

float BVHTree::Intersect(const Ray& ray)
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
				const int primOffset = node->primOffset, finalOffset = node->primOffset + node->NumPrimitives;
				for (int i = primOffset; i < finalOffset; i++)
				{
					const Object* object = objects[primitives[i].objectIndex];
					const float t = object->isPrimitiveHitByRay(ray, primitives[i].primitiveIndex);
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


void BVHTree::PrintTree()
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

void BVHTree::LogPrimitivesInfo(const std::vector<ObjectBVHInfo>& primsInfo, std::string path)
{
	std::ofstream file(path);

	file << "number of primitives: " << primsInfo.size() << std::endl;

	for (int i = 0; i < primsInfo.size(); ++i)
	{
		const ObjectBVHInfo& primInfo = primsInfo[i];
		file << "primitive["<<i<<"]:\n\tbounds:\n\t\tmin: " << primInfo.bounds.min().transpose() << 
				"\n\t\tmax: " << primInfo.bounds.max().transpose() << "\n\tcentroid: "<<primInfo.centroid.transpose() << std::endl;
	}
	file.close();
}

void BVHTree::PrintTree(std::string file_path)
{
	std::ofstream file(file_path);
	int count = 0, currentNode = 0;

	std::vector<int> VisitStack;
	VisitStack.reserve(128);

	file << "node count: " << nodeArray.size() << std::endl;
	file << "number of primitives: " << primitives.size() << std::endl;
	if (nodeArray[0].NumPrimitives > 0)
	{
		file << "node[0] is a leaf node and contains all the primitives." << std::endl;
		file.close();
		return;
	}

	while (true)
	{
		BVHTreeNode* node = &nodeArray[currentNode];

		/*std::cout << "node " << currentNode << std::endl;
		std::cout << "nObjects: " << node->NumPrimitives << std::endl;
		std::cout << "bounds:" << std::endl;
		std::cout << "	Min: " << node->Bounds.min().transpose() << std::endl;
		std::cout << "	Max: " << node->Bounds.max().transpose() << std::endl;*/
		file << "node["<< currentNode << "] bounds: \n\tmin: " << node->Bounds.min().transpose() << "\n\tmax: " << node->Bounds.max().transpose() << std::endl;

		if (node->NumPrimitives > 0)
		{
			file << "node["<< currentNode << "] primitives: " << node->NumPrimitives << std::endl;
			if (VisitStack.size() == 0) break;
			currentNode = VisitStack.back();
			VisitStack.pop_back();
		}
		else
		{
			file << "node["<< currentNode << "] left child: " << currentNode + 1 << "; right child: " << nodeArray[currentNode].SecondChildOffset << std::endl;
			currentNode = currentNode + 1;
			VisitStack.push_back(node->SecondChildOffset);
		}

		//if (VisitStack.size() == 0) break;

	}

	file.close();
	return;
}