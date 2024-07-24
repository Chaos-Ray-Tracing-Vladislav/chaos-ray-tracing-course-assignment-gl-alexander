#pragma once
#include "CRTVector.h"

enum class RayType {
	CAMERA, 
	SHADOW, 
	REFLECTIVE,
	REFRACTIVE,
	LIGHT
};

struct CRTRay
{
	CRTVector origin;
	CRTVector direction;
	RayType type;
	int depth;
};

