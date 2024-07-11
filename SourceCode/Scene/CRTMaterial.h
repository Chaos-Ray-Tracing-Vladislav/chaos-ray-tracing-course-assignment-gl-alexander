#pragma once
#include "../Utils/CRTVector.h"
#include "Textures/Texture.h"
#include <memory>
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
	std::shared_ptr<Texture> texture;
	bool smoothShading;
	float ior;
};

