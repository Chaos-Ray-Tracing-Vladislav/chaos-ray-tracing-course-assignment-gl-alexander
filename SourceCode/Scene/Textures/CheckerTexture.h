#pragma once
#include "Texture.h"

class CheckerTexture : public Texture
{
	CRTVector colorA;
	CRTVector colorB;
	float squareSize;
public:
	CheckerTexture(const std::string& name, const CRTVector& colorA, const CRTVector& colorB, float squareSize);
	CRTVector sample(float u, float v) const override;
};
