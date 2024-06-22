#pragma once
#include <vector>
#include <tuple>
#include "../Utils/CRTTriangle.h"
#include "../Utils/CRTRay.h"

class CRTMesh
{
	std::vector<CRTVector> vertices;
	std::vector<int> triangleVertIndices;
public:
	CRTMesh(const std::vector<CRTVector>& vertices, const std::vector<int>& triangleVertIndices);

	std::tuple<bool, CRTVector, CRTTriangle> intersectsRay(const CRTRay& ray) const;
	bool intersectsShadowRay(const CRTRay& ray) const;
};

