#pragma once
#include "CRTVector.h"
#include "CRTRay.h"

struct CRTBox {
	CRTVector min;
	CRTVector max;

	bool intersects(const CRTRay& ray) const;
};

