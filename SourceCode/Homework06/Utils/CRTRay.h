#pragma once
#include <iostream>
#include "CRTVector.h"
#include "CRTTriangle.h"

class CRTRay
{
	CRTVector origin;
	CRTVector direction;
public: 
	CRTRay();
	CRTRay(const CRTVector& origin, const CRTVector& direction);

	const CRTVector& getOrigin() const;
	const CRTVector& getDirection() const;
	std::pair<bool, CRTVector> intersectsTriangle(const CRTTriangle& triangle) const;
};

