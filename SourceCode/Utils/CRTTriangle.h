#pragma once
#include "CRTVector.h"
#include "CRTColor.h"
#include "CRTRay.h"
#include <cmath>
#include <vector>
#include <utility>

const int VERTICES = 3;

class CRTTriangle
{
	CRTVector vertices[VERTICES];
	CRTVector normal;
	void calculateNormalVector();

public:
	CRTTriangle() = default;
	CRTTriangle(const CRTVector vertices[VERTICES]);
	CRTTriangle(const CRTVector vertices[VERTICES], const CRTVector& normal); // to avoid calculating the normal in case we already have it
	CRTTriangle(const CRTVector& v1, const CRTVector& v2, const CRTVector& v3);
	CRTTriangle(const CRTVector& v1, const CRTVector& v2, const CRTVector& v3, const CRTVector& normal);


	const CRTVector& getNormal() const;
	std::vector<CRTVector> getVertices() const;

	float area() const;
	float distanceToPoint(const CRTVector& point) const;

	bool pointInTriangle(const CRTVector& point) const;
	std::pair<bool, CRTVector> intersectsRay(const CRTRay& ray) const;
	bool intersectsShadowRay(const CRTRay& ray) const;

	CRTVector getBarycenticCoordinates(const CRTVector& point) const;
};
