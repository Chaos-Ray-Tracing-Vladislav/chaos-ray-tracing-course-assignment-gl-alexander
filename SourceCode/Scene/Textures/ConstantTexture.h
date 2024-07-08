#pragma once
#include "Texture.h"

class ConstantTexture : public Texture
{
	CRTVector color;
public:
	ConstantTexture(const std::string& name, const CRTVector& color);
	CRTVector sample(float u, float v) const override;
};