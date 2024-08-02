#pragma once
#include <vector>
#include "CRTVector.h"

using CRTImage = std::vector<std::vector<CRTVector>>;

// aggressive
constexpr int FXAA_SEARCH_STEPS = 16;

constexpr float FXAA_EDGE_THRESHOLD = 0.0625;
constexpr float FXAA_EDGE_THRESHOLD_MIN = 0.012;
constexpr float FXAA_SUBPIX_TRIM = 1.0 / 8;
constexpr float FXAA_SUBPIX_CAP = 1.0f;


struct FXAA
{
	static float FXAALuminance(const CRTVector& rgb);
	static float FXAAColorContrast(const CRTVector& a, const CRTVector& b);
	static CRTVector FXAAPixel(CRTImage& image, unsigned x, unsigned y);
	static void applyFXAA(CRTImage& image);
};

