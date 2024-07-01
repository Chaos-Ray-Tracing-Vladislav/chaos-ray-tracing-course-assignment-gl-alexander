#pragma once
#include "../Utils/CRTVector.h"

enum class CRTMaterialType {
	DIFFUSE,
	REFLECTIVE
};

struct CRTMaterial {
	CRTMaterialType type;
	CRTVector albedo;
	bool smoothShading;
};

