#include "CRTRay.h"

CRTRay::CRTRay() : CRTRay({ 0, 0, 0 }, { 0, 0, 0 }) {}
CRTRay::CRTRay(const CRTVector& origin, const CRTVector& direction) : origin(origin), direction(direction) {}

const CRTVector& CRTRay::getOrigin() const {
	return origin;
}
const CRTVector& CRTRay::getDirection() const {
	return direction;
}

std::pair<bool, CRTVector> CRTRay::intersectsTriangle(const CRTTriangle& triangle) const {
	CRTVector normalVector = triangle.getNormal();
	float dotPr = dot(normalVector, direction);
	if (dotPr >= 0) {
		// this means it's either parallel or 'behind' the camera
		return { false, CRTVector() };
	}
	// to find the distance from the ray origin to the triangle we can pick any of its vertices and
	// use geometry to calculate it with the dot product
	float distanceToPlane = triangle.distanceToPoint(origin);
	dotPr *= -1;
	CRTVector hitPoint = direction * (distanceToPlane / dotPr);	// the dotProduct is a projection
	if (triangle.pointInTriangle(hitPoint)) {
		return { true, hitPoint };
	}
	return { false, hitPoint };
}
