#include "ConstantTexture.h"

ConstantTexture::ConstantTexture(const CRTVector& color) : color(color)
{}

CRTVector ConstantTexture::sample(float u, float v, const CRTVector& bary) const
{
	return color;
}
