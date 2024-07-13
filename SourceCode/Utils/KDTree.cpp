#include "KDTree.h"
#include <iostream>


KDTree::KDTree(const std::vector<CRTMesh>& objects, const std::vector<CRTMaterial>& materials, const CRTBox& sceneAABB) 
	: objects(objects), materials(materials)
{
	NodeTriangles allTriangles;
	int currTrianglesCount = 0;
	objectTriangleCount.push_back(currTrianglesCount);
	for (auto& mesh : objects) {
		const std::vector<CRTTriangle> meshTriangles = std::move(mesh.getAllTriangles());
		for (int i = 0; i < meshTriangles.size(); i++) {
			allTriangles.push_back({ meshTriangles[i], i + currTrianglesCount });
		}
		currTrianglesCount += meshTriangles.size();
		objectTriangleCount.push_back(currTrianglesCount);
	}
	int leafNodesCount = allTriangles.size() / MAX_TRIANGLES; // if all leaf buckets are filled with MAX_TRIANGES 
	// this implementation of a KD tree is binary, but the maximum number of nodes for a full k-tree with n leaf nodes is k^n
	// if there are less triangles than MAX_TRIANGLES, then leafNodes is 0 and all triangles will be in the root box
	int maxNodesCount = (2 << leafNodesCount);
	nodes.reserve(maxNodesCount);
	nodes.emplace_back(sceneAABB, INVALID_IND, INVALID_IND, INVALID_IND, NodeTriangles());
	build(0, allTriangles, 0);
}

// parentInd is the node whose children we're creating, depth is the current recursion depth
void KDTree::build(int parentInd, const NodeTriangles& remainingTriangles, int depth)
{
	treeDepth = std::max(treeDepth, depth);
	if (remainingTriangles.size() <= MAX_TRIANGLES || depth > MAX_DEPTH) {
		nodes[parentInd].triangles = remainingTriangles;
		return;
	}

	CRTBox first, second;
	nodes[parentInd].box.split(first, second, (AxisLabel)(depth % AXIS_COUNT));

	NodeTriangles firstTriangles;
	NodeTriangles secondTriangles;
	firstTriangles.reserve(remainingTriangles.size() / 2);
	secondTriangles.reserve(remainingTriangles.size() / 2);

	for (auto& tri : remainingTriangles) {
		if (tri.triangle.isInBox(first)) {
			firstTriangles.push_back(tri);
		}
		if (tri.triangle.isInBox(second)) {
			secondTriangles.push_back(tri);
		}
	}
	if (firstTriangles.size() > 0) {
		int firstInd = nodes.size();
		nodes.emplace_back(first, parentInd, INVALID_IND, INVALID_IND, NodeTriangles());
		nodes[parentInd].leftInd = firstInd;
		build(firstInd, firstTriangles, depth + 1);
	}
	if (secondTriangles.size() > 0) {
		int secondInd = nodes.size();
		nodes.emplace_back(second, parentInd, INVALID_IND, INVALID_IND, NodeTriangles());
		nodes[parentInd].rightInd = secondInd;
		build(secondInd, secondTriangles, depth + 1);
	}
	
}

// triangleIndex is the total index of the triangle in the scene, as stored in TriangleIndexPair
void KDTree::getMeshAndRelativeIndex(int triangleIndex, int& meshIndex, int& relativeIndex) const
{
	// in order to find the index of the mesh this triangle is part of, we can use its index
	// knowing that the meshes are in a fixed order, if for example meshes[0] has 30 triangles, meshes[1] has 12 and meshes[2] has 45
	// we can build the objectTriangleCount by adding the number of meshes in total in the scene for each mesh in order
	// for the above example the array would look like this: [0, 30, 42, 87]
	// so when searching for triangle with index 37, we can just do std::lower_bound and find it in log(#meshes) time

	auto it = std::lower_bound(objectTriangleCount.begin(), objectTriangleCount.end(), triangleIndex);
	meshIndex = std::distance(objectTriangleCount.begin(), it);
	if (objectTriangleCount[meshIndex] != triangleIndex) {
		meshIndex--;
		// this is because lower bound returns the first element that is not less than triangleIndex
		// so for triangleIndex 34 it would return an iterator to 42, or meshIndex 2, when in reality it should be 1 
	}
	relativeIndex = triangleIndex - objectTriangleCount[meshIndex];
}

Intersection KDTree::intersectLeaf(const CRTRay& ray, const NodeTriangles& triangles, float maxDistance) const
{
	Intersection intersection;
	int meshIndex = 0, relativeIndex = 0;
	float closestHitDitance = FLOAT_MAX;
	Intersection triangle_intersection;
	for (auto& tri : triangles) {
		triangle_intersection = std::move(tri.triangle.intersectsRay(ray));
		if (triangle_intersection.triangleIndex != NO_HIT_INDEX) {
			float distance = (triangle_intersection.hitPoint - ray.origin).length();
			if (distance < closestHitDitance && distance <= maxDistance) {
				getMeshAndRelativeIndex(tri.index, meshIndex, relativeIndex);
				closestHitDitance = distance;
				intersection = std::move(triangle_intersection); // this copies all the data we've already calculated
				intersection.triangleIndex = relativeIndex;
				intersection.hitObjectIndex = meshIndex;
			}
		}
	}
	if (intersection.triangleIndex != NO_HIT_INDEX) {
		intersection.smoothNormal = objects[meshIndex].calculateSmoothNormal(
			intersection.triangleIndex, intersection.barycentricCoordinates, intersection.hitPoint);
		intersection.materialIndex = objects[meshIndex].getMaterialIndex();
		return intersection;
	}
	
}

Intersection KDTree::intersect(const CRTRay& ray, float maxDistance) const
{
	std::vector<Intersection> allIntersections;
	Intersection currIntersection;
	std::stack<int> nodeStack;
	int currInd = 0;
	nodeStack.push(currInd);
	while (!nodeStack.empty()) {
		currInd = nodeStack.top();
		nodeStack.pop();
		if (nodes[currInd].box.intersects(ray)) {
			if (nodes[currInd].triangles.size() > 0) { // leaf node
				currIntersection = std::move(intersectLeaf(ray, nodes[currInd].triangles, maxDistance));
				if (currIntersection.triangleIndex != NO_HIT_INDEX) {
					allIntersections.push_back(currIntersection);
				}
			}
			else { // adds children nodes
				if (nodes[currInd].leftInd != INVALID_IND)
					nodeStack.push(nodes[currInd].leftInd);
				if (nodes[currInd].rightInd != INVALID_IND)
					nodeStack.push(nodes[currInd].rightInd);
			}
		}
	}
	Intersection result;
	float minDistance = FLOAT_MAX;
	float currDistance;
	for (auto& intersection : allIntersections) {
		currDistance = (intersection.hitPoint - ray.origin).length();
		if (currDistance < minDistance && currDistance <= maxDistance) {
			minDistance = currDistance;
			result = std::move(intersection);
		}
	}
	return result;
}


TypeIntersectionMap KDTree::intersectPerTypeLeaf(const CRTRay& ray,
	const NodeTriangles& triangles,
	float maxDist) const
{
	TypeIntersectionMap result;
	std::unordered_map<CRTMaterialType, float> closestDistance;
	int meshIndex = 0, relativeIndex = 0;
	float currDistance;
	Intersection triangle_intersection;
	for (auto& tri : triangles) {
		triangle_intersection = std::move(tri.triangle.intersectsRay(ray));
		if (triangle_intersection.triangleIndex != NO_HIT_INDEX) {
			currDistance = (triangle_intersection.hitPoint - ray.origin).length();
			if (currDistance > maxDist) continue;

			const CRTMaterial& mat = materials[objects[meshIndex].getMaterialIndex()];
			if (result.count(mat.type)) { // there's already a material with that type that's been intersected
				if (currDistance < closestDistance[mat.type]) {
					getMeshAndRelativeIndex(tri.index, meshIndex, relativeIndex);
					closestDistance[mat.type] = currDistance;
					result[mat.type] = std::move(triangle_intersection);
					result[mat.type].triangleIndex = relativeIndex;
					result[mat.type].hitObjectIndex = meshIndex;
				}
			}
			else {

				getMeshAndRelativeIndex(tri.index, meshIndex, relativeIndex);
				closestDistance[mat.type] = currDistance;
				result[mat.type] = std::move(triangle_intersection);
				result[mat.type].triangleIndex = relativeIndex;
				result[mat.type].hitObjectIndex = meshIndex;
			}

		}
	}
	return result;
}

void KDTree::handleLeafIntersections(TypeIntersectionMap& resultIntersections, TypeIntersectionMap&& leafIntersections,
	const CRTRay& ray) const
{
	// leafIntersections is a rval reference, because it's discarded after taking this information
	// so we can save copy operations when taking intersection values with std::move
	for (auto& it : leafIntersections) {
		if (resultIntersections.count(it.first)) { // so there's already an intersection with the given material type
			if ((it.second.hitPoint - ray.origin).length() < (resultIntersections[it.first].hitPoint - ray.origin).length()) {
				// then we've found a closer  hit for that material type
				resultIntersections[it.first] = std::move(it.second);
			}
		}
		else {
			resultIntersections[it.first] = std::move(it.second);
		}
	}
}

TypeIntersectionMap KDTree::intersectPerType(const CRTRay& ray, float maxDist) const
{
	TypeIntersectionMap result;
	TypeIntersectionMap leafIntersections;

	std::stack<int> nodeStack;
	int currInd = 0;
	nodeStack.push(currInd);
	while (!nodeStack.empty()) {
		currInd = nodeStack.top();
		nodeStack.pop();
		if (nodes[currInd].box.intersects(ray)) {
			if (nodes[currInd].triangles.size() > 0) {
				handleLeafIntersections(result, intersectPerTypeLeaf(ray, nodes[currInd].triangles, maxDist), ray);
			}
			else { // adds children nodes
				if (nodes[currInd].leftInd != INVALID_IND)
					nodeStack.push(nodes[currInd].leftInd);
				if (nodes[currInd].rightInd != INVALID_IND)
					nodeStack.push(nodes[currInd].rightInd);
			}
		}
	}
	//handleLeafIntersections takes into account the closest hit per type
	return result;
}
