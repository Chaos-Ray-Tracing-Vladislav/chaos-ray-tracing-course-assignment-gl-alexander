#pragma once
#include "../../Utils/CRTVector.h"

class EdgeTexture {
    CRTVector edgeColor;
    CRTVector innerColor;
    float edgeWidth;

public:
    EdgeTexture( const CRTVector& edgeColor, const CRTVector& innerColor, float edgeWidth);

    CRTVector sample(float u, float v, const CRTVector& bary) const;
};
