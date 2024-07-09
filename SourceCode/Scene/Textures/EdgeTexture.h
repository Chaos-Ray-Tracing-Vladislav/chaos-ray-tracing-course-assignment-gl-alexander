#pragma once
#include "Texture.h"

class EdgeTexture : public Texture {
    CRTVector edgeColor;
    CRTVector innerColor;
    float edgeWidth;

public:
    EdgeTexture(const std::string& name, const CRTVector& edgeColor, const CRTVector& innerColor, float edgeWidth);

    CRTVector sample(float u, float v, const CRTVector& bary) const override;
};
