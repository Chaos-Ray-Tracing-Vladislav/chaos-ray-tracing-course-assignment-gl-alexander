#include "CheckerTexture.h"

CheckerTexture::CheckerTexture(const CRTVector& colorA, const CRTVector& colorB, float squareSize)
	: colorA(colorA), colorB(colorB), squareSize(squareSize)
{}

CRTVector CheckerTexture::sample(float u, float v, const CRTVector& bary) const
{
	unsigned squareX = u / squareSize;
	unsigned squareY = v / squareSize;
	if (squareX % 2 == squareY % 2) {
		return colorA;
	}
	return colorB;
}
