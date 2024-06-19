#pragma once
#include <vector>
#include "CRTVector.h"
#include "CRTColor.h"
#include "CRTRay.h"


const float PIXEL_LENGTH = 1.0;

using CRTRayMatrix = std::vector<std::vector<CRTRay>>;

// represents a screen of pixels in the world, parallel to the XY plane
class CRTScreen
{
	CRTVector position; // these are the world coordinates for the image center point
	unsigned width;
	unsigned height;

	CRTRay getRayForPixel(unsigned rowId, unsigned colId, const CRTVector& cameraPosition) const;
public: 
	CRTScreen(const CRTVector& position, unsigned width, unsigned height);
	unsigned getWidth() const;
	unsigned getHeight() const;

	CRTRayMatrix generateRays(const CRTVector& cameraPosition) const;	
};

