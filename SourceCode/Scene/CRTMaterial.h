#pragma once
#include "../Utils/CRTVector.h"
#include <string>

enum class CRTMaterialType {
	DIFFUSE,
	REFLECTIVE,
	REFRACTIVE
};

struct CRTMaterial {
	CRTMaterialType type;
	std::string textureName;
	bool smoothShading;
	float ior;
};

