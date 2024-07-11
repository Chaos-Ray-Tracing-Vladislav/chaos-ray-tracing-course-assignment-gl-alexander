#pragma once
#include "../../Utils/CRTVector.h"

class ConstantTexture
{
	CRTVector color;
public:
	ConstantTexture(const CRTVector& color);
	CRTVector sample(float u, float v, const CRTVector& bary) const;
};