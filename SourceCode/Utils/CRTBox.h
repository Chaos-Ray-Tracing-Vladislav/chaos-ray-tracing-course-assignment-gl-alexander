#pragma once
#include "CRTVector.h"
#include "CRTRay.h"
#include <limits>

static constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
static constexpr float FLOAT_MIN = -FLOAT_MAX;

struct CRTBox {
	CRTVector min;
	CRTVector max;

	bool intersects(const CRTRay& ray) const;
	void split(CRTBox& first, CRTBox& second, AxisLabel label) const;
};

