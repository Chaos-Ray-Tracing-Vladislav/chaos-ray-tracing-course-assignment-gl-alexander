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
        // Ray is parallel to the plane or facing away from the triangle
        return { false, CRTVector() };
    }

    // Calculate the signed distance from the ray origin to the plane
    float distanceToPlane = -(dot(normalVector, origin - triangle.getVertices()[0])) / dotPr;

    if (distanceToPlane < 0) {
        // The intersection is behind the ray origin
        return { false, CRTVector() };
    }

    // Compute the hit point
    CRTVector hitPoint = origin + (direction * distanceToPlane);

    if (triangle.pointInTriangle(hitPoint)) {
        return { true, hitPoint };
    }

    return { false, hitPoint };
}
