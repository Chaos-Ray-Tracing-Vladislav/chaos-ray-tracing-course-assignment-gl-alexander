#include "CRTLight.h"

CRTLight::CRTLight(const CRTVector& position, float intensity) : position(position), intensity(intensity)
{}

const CRTVector& CRTLight::getPosition() const
{
	return position;
}

float CRTLight::getIntensity() const
{
	return intensity;
}
