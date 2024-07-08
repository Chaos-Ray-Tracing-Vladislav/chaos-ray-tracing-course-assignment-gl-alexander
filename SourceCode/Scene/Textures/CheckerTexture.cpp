#include "CheckerTexture.h"

CheckerTexture::CheckerTexture(const std::string& name, const CRTVector& colorA, const CRTVector& colorB, float squareSize)
	: Texture(name), colorA(colorA), colorB(colorB), squareSize(squareSize)
{}

CRTVector CheckerTexture::sample(float u, float v) const
{
	unsigned squareX = u / squareSize;
	unsigned squareY = v / squareSize;
	if (squareX % 2 == squareY % 2) {
		return colorA;
	}
	return colorB;
}
