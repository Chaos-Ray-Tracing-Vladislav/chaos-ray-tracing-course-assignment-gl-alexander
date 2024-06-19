#pragma once
#include <vector>
#include "Utils/CRTVector.h"
#include "Utils/CRTColor.h"
#include "Utils/CRTRay.h"
#include "CRTCamera.h"


const float PIXEL_LENGTH = 1.0;

using CRTRayMatrix = std::vector<std::vector<CRTRay>>;

// represents a screen of pixels in the world, parallel to the XY plane
class CRTScreen
{
	CRTCamera camera;
	CRTVector offset; // the offset from the camera with applied rotation
	unsigned width;
	unsigned height;

	CRTRay getRayForPixel(unsigned rowId, unsigned colId) const;
public: 
	CRTScreen(const CRTCamera& camera, unsigned width, unsigned height);
	CRTScreen(const CRTCamera& camera, const CRTVector& offset, unsigned width, unsigned height);
	unsigned getWidth() const;
	unsigned getHeight() const;
	const CRTCamera& getCamera() const;

	CRTRayMatrix generateRays() const;	
};

