#pragma once
#include <vector>
#include <unordered_map>
#include <stack>
#include "../Scene/CRTMesh.h"
#include "../Scene/CRTMaterial.h"
#include "CRTBox.h"
#include "CRTTriangle.h"

static const int INVALID_IND = -1;

static const int MAX_DEPTH = 40;
static const int MAX_TRIANGLES = 16;

struct KDTreeNode {
	struct TriangleIndexPair {
		CRTTriangle triangle;
		int index = INVALID_IND;
	};
	CRTBox box;
	int parentInd = INVALID_IND;
	int leftInd = INVALID_IND;
	int rightInd = INVALID_IND;
	std::vector<TriangleIndexPair> triangles;
};

using NodeTriangles = std::vector<KDTreeNode::TriangleIndexPair>;

class KDTree {
	std::vector<KDTreeNode> nodes;
	int treeDepth;
	std::vector<int> objectTriangleCount;
	const std::vector<CRTMesh>& objects;
	const std::vector<CRTMaterial>& materials;

	// calculates the index of the mesh and the relative index of the triangle in the mesh, by a given absolute triangle Index
	void getMeshAndRelativeIndex(int triangleIndex, int& meshIndex, int& relativeIndex) const;

	Intersection intersectLeaf(const CRTRay& ray, const NodeTriangles& triangles, float maxDist = FLOAT_MAX) const;

	void build(int parentInd, const NodeTriangles& remainingTriangles, int depth);
public:
	KDTree(const std::vector<CRTMesh>& objects, const std::vector<CRTMaterial>& materials, const CRTBox& sceneAABB);

	// returns the closest intersection to the ray origin
	Intersection intersect(const CRTRay& ray, float maxDist = FLOAT_MAX) const;
};