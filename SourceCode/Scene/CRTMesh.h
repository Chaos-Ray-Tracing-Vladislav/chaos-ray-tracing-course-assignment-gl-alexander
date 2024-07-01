#pragma once
#include <vector>
#include <tuple>
#include "../Utils/CRTTriangle.h"
#include "../Utils/CRTRay.h"
#include "CRTMaterial.h"

static const CRTMaterial DEFAULT_MATERIAL{ CRTMaterialType::DIFFUSE, {0,0,1}, false };

class CRTMesh
{
	std::vector<CRTVector> vertices;
	std::vector<int> triangleVertIndices;
	std::vector<CRTVector> vertexNormals;
	std::vector<CRTVector> faceNormals;
	CRTMaterial material;

	void calculateFaceNormals();
	void calculateVertexNormals();

	CRTVector calculateSmoothNormal(int triangleIndex, const CRTVector& point) const;
public:
	CRTMesh(const std::vector<CRTVector>& vertices, const std::vector<int>& triangleVertIndices);
	CRTMesh(const std::vector<CRTVector>& vertices, const std::vector<int>& triangleVertIndices, const CRTMaterial& material);

	std::tuple<bool, CRTVector, CRTVector> intersectsRay(const CRTRay& ray) const;
	std::tuple<bool, CRTVector, CRTVector, CRTVector> intersectsRayBarycentic(const CRTRay& ray) const;
	bool intersectsShadowRay(const CRTRay& ray) const;

	CRTMaterial getMaterial() const;
};

