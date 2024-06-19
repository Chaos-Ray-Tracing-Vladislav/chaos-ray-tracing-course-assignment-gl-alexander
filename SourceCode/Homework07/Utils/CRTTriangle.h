#pragma once
#include "CRTVector.h"
#include "CRTColor.h"
#include <cmath>

const int VERTICES = 3;

class CRTTriangle
{
	CRTVector vertices[VERTICES];
	CRTVector normal;
	CRTColor color = {MAX_COLOR_COMPONENT, MAX_COLOR_COMPONENT, MAX_COLOR_COMPONENT};
	void calculateNormalVector();

public:
	CRTTriangle() = default;
	CRTTriangle(const CRTVector vertices[VERTICES]);
	CRTTriangle(const CRTVector vertices[VERTICES], const CRTColor& color);
	CRTTriangle(const CRTVector& v1, const CRTVector& v2, const CRTVector& v3);
	CRTTriangle(const CRTVector& v1, const CRTVector& v2, const CRTVector& v3, const CRTColor& color);

	const CRTVector& getNormal() const;
	const CRTVector* getVertices() const;
	const CRTColor& getColor() const;

	float area() const;
	float distanceToPoint(const CRTVector& point) const;

	bool pointInTriangle(const CRTVector& point) const;
};

