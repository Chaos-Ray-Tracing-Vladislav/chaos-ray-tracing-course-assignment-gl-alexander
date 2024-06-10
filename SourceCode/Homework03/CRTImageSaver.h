#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include "CRTColor.h"

typedef std::vector<std::vector<CRTColor>> CRTImage;

const unsigned MAX_COLOR_COMPONENT = 255;

static const char LINE_SEP = '\n';
static const char PIXEL_SEP = '\t';

class CRTImageSaver
{
public:
	static void saveImage(const char* imgName, const CRTImage& image);
	static void saveImage(std::ofstream& ppm, const CRTImage& image);
};

std::ofstream& operator<<(std::ofstream& ppm, const CRTColor& pixel);