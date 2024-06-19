#pragma once
#include <vector>
#include "../Utils/CRTTriangle.h"

class CRTMesh
{
	std::vector<CRTVector> vertices;
	std::vector<int> triangleVertIndices;
public:
	CRTMesh(const std::vector<CRTVector>& vertices, const std::vector<int>& triangleVertIndices);

	std::vector<CRTTriangle> getTriangles() const;
};

