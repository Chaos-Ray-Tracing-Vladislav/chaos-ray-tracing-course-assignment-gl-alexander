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
	bool constantAlbedo;
	CRTVector albedo;
	std::string textureName;
	bool smoothShading;
	float ior;
};

