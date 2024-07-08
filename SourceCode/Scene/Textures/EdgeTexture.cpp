#include "EdgeTexture.h"

EdgeTexture::EdgeTexture(const std::string& name, const CRTVector& edgeColor, const CRTVector& innerColor, float edgeWidth)
	: Texture(name), edgeColor(edgeColor), innerColor(innerColor), edgeWidth(edgeWidth)
{}

CRTVector EdgeTexture::sample(float u, float v) const
{
	if (u < edgeWidth || v < edgeWidth || abs(1 - u - v) < edgeWidth || 1 - u < edgeWidth || 1 - v < edgeWidth) {
		return edgeColor;
	}

	return innerColor;
}
