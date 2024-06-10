#pragma once
#include "CRTVector.h"

class CRTRay
{
	CRTVector origin;
	CRTVector direction;
public: 
	CRTRay();
	CRTRay(const CRTVector& origin, const CRTVector& direction);

	const CRTVector& getOrigin() const;
	const CRTVector& getDirection() const;
};

