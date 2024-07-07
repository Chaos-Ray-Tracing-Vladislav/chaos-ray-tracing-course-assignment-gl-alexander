#include "ConstantTexture.h"

ConstantTexture::ConstantTexture(const std::string& name, const CRTVector& color) : Texture(name), color(color)
{}

CRTVector ConstantTexture::sample(float u, float v) const
{
	return color;
}
