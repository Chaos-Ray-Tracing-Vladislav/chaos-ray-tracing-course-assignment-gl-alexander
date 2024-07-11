#pragma once
#include "../../Utils/CRTVector.h"

class CheckerTexture
{
	CRTVector colorA;
	CRTVector colorB;
	float squareSize;
public:
	CheckerTexture(const CRTVector& colorA, const CRTVector& colorB, float squareSize);
	CRTVector sample(float u, float v, const CRTVector& bary) const;
};
