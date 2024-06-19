#include "CRTRay.h"

CRTRay::CRTRay() : CRTRay({ 0, 0, 0 }, { 0, 0, 0 }) {}
CRTRay::CRTRay(const CRTVector& origin, const CRTVector& direction) : origin(origin), direction(direction) {}

const CRTVector& CRTRay::getOrigin() const {
	return origin;
}
const CRTVector& CRTRay::getDirection() const {
	return direction;
}

