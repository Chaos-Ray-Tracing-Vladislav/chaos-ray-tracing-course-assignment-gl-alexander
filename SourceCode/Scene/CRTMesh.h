#pragma once
#include <vector>
#include "../Utils/CRTTriangle.h"
#include "../Utils/CRTRay.h"
#include "CRTMaterial.h"
#include "Intersection.h"
#include <unordered_map>

class CRTMesh
{
	std::vector<CRTVector> vertices;
	std::vector<CRTVector> uvs;
	std::vector<int> triangleVertIndices;
	std::vector<CRTVector> vertexNormals;
	std::vector<CRTVector> faceNormals;
	std::unordered_map<int, std::vector<int>> vertexTriangleParticipation; // for each vertex stores the triangles it's a part of
	int materialIndex;

	void calculateFaceNormals();
	void calculateVertexNormals();

	CRTVector calculateSmoothNormal(int triangleIndex, const CRTVector& barycentic, const CRTVector& point) const;
public:
	CRTMesh(const std::vector<CRTVector>& vertices, const std::vector<int>& triangleVertIndices, int materialIndex);
	CRTMesh(const std::vector<CRTVector>& vertices, const std::vector<CRTVector>& uvs, const std::vector<int>& triangleVertIndices, int materialIndex);

	Intersection intersectsRay(const CRTRay& ray) const;

	CRTVector getUV(const Intersection& data) const;
	CRTVector sampleMaterial(const CRTMaterial& material, const Intersection& data) const;

	int getMaterialIndex() const;
};

