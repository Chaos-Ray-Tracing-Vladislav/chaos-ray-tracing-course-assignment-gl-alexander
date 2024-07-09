#include "EdgeTexture.h"

EdgeTexture::EdgeTexture(const std::string& name, const CRTVector& edgeColor, const CRTVector& innerColor, float edgeWidth)
	: Texture(name), edgeColor(edgeColor), innerColor(innerColor), edgeWidth(edgeWidth)
{}

CRTVector EdgeTexture::sample(float u, float v, const CRTVector& bary) const
{
	if (bary.x < edgeWidth || bary.y < edgeWidth || abs(bary.z) < edgeWidth || 1 - bary.x < edgeWidth || 1 - bary.y < edgeWidth) {
		return edgeColor;
	}
	return innerColor;
}
