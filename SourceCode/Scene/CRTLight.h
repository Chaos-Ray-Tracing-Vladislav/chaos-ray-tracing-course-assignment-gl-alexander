#pragma once
#include "../Utils/CRTVector.h"

class CRTLight {
	CRTVector position;
	float intensity;
public:
	CRTLight(const CRTVector& position, float intensity);

	const CRTVector& getPosition() const;
	float getIntensity() const;
};