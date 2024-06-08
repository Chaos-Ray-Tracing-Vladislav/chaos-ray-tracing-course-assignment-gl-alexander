#pragma once
#include "CRTVector.h"
#include <cmath>

static const int VERTICES = 3;

class CRTTriangle
{
	CRTVector vertices[VERTICES];
public:
	CRTTriangle() = default;
	CRTTriangle(const CRTVector vertices[VERTICES]);
	CRTTriangle(const CRTVector& v1, const CRTVector& v2, const CRTVector& v3);

	CRTVector normalVector() const;
	float area() const;
};

