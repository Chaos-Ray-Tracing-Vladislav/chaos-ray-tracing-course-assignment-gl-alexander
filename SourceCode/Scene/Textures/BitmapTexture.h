#pragma once
#include <vector>
#include "../../Utils/CRTVector.h"
#include "../../Utils/CRTColor.h"

class BitmapTexture
{
	int width;
	int height;
	int channels;
	std::vector<CRTVector> pixels;
public:
	BitmapTexture(const std::string& imagePath);
	CRTVector sample(float u, float v, const CRTVector& bary) const ;
};

