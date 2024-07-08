#pragma once
#include "../Utils/CRTVector.h"

enum class CRTMaterialType {
	DIFFUSE,
	REFLECTIVE,
	REFRACTIVE
};

struct CRTMaterial {
	CRTMaterialType type;
	CRTVector albedo;
	bool smoothShading;
	float ior;
};

