#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include "CRTColor.h"
#include "CRTVector.h"

using CRTImage = std::vector<std::vector<CRTVector>>;

constexpr float EDGE_THRESHOLD_MIN = 0.0312;
constexpr float EDGE_THRESHOLD_MAX = 0.125;

static const char LINE_SEP = '\n';
static const char PIXEL_SEP = '\t';

class CRTImageSaver
{
public:
	static void saveImage(const char* imgName, const CRTImage& image, bool FXAA = false);
	static void saveImage(std::ofstream& ppm, const CRTImage& image, bool FXAA = false);
};

std::ofstream& operator<<(std::ofstream& ppm, const CRTColor& pixel);