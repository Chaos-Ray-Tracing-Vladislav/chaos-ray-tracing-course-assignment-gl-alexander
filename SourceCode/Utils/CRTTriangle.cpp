#include "CRTTriangle.h"

CRTTriangle::CRTTriangle(const CRTVector vertices[VERTICES]) : CRTTriangle(vertices[0], vertices[1], vertices[2]) 
{
}

CRTTriangle::CRTTriangle(const CRTVector vertices[VERTICES], const CRTVector& normal) : CRTTriangle(vertices[0], vertices[1], vertices[2], normal) 
{
}


CRTTriangle::CRTTriangle(const CRTVector& v1, const CRTVector& v2, const CRTVector& v3) {
	this->vertices[0] = v1;
	this->vertices[1] = v2;
	this->vertices[2] = v3;
	calculateNormalVector();
}


CRTTriangle::CRTTriangle(const CRTVector& v1, const CRTVector& v2, const CRTVector& v3, const CRTVector& normal) {
	this->vertices[0] = v1;
	this->vertices[1] = v2;
	this->vertices[2] = v3;
	this->normal = normal;
}


/* the order of the vertices matters
		v2


	v0		v1
*/
void CRTTriangle::calculateNormalVector(){
	CRTVector v1 = vertices[1] - vertices[0];
	CRTVector v2 = vertices[2] - vertices[0];

	normal = cross(v1, v2); // we set it to the counterclockwise cross-product
	normal.normalize();
}

const CRTVector& CRTTriangle::getNormal() const {
	return this->normal;
}

std::vector<CRTVector> CRTTriangle::getVertices() const
{
	std::vector<CRTVector> vert = { vertices[0], vertices[1], vertices[2] };
	return vert;
}


float CRTTriangle::area() const {
	CRTVector v1 = vertices[1] - vertices[0];
	CRTVector v2 = vertices[2] - vertices[0];

	return cross(v1, v2).length() / 2;
}

float CRTTriangle::distanceToPoint(const CRTVector& point) const {
	// it doesn't matter which vertex we pick to calculate from
	return dot(normal, vertices[0] - point);
}


bool CRTTriangle::pointInTriangle(const CRTVector& point) const {
	CRTVector E0 = vertices[1] - vertices[0];
	CRTVector E1 = vertices[2] - vertices[1];
	CRTVector E2 = vertices[0] - vertices[2];

	CRTVector relativeE0 = cross(E0, point - vertices[0]);
	CRTVector relativeE1 = cross(E1, point - vertices[1]);
	CRTVector relativeE2 = cross(E2, point - vertices[2]);

	// all the cross products need to be oriented the same way as the normal vector

	return  dot(normal, relativeE0) >= -EPSILON && dot(normal, relativeE1) >= -EPSILON && dot(normal, relativeE2) >= -EPSILON;
}

Intersection CRTTriangle::intersectsRay(const CRTRay& ray) const {
	Intersection intersection;
	intersection.triangleIndex = NO_HIT_INDEX;

	float dotPr = dot(normal, ray.direction);
	if (ray.type == RayType::CAMERA && dotPr >= 0) {
		// back face culling for camera rays only
		// Ray is parallel to the plane or facing away from the triangle
		return intersection;
	}

	// Calculate the signed distance from the ray origin to the plane
	float d = -dot(normal, vertices[0]); // the D in the ray equation

	float t = -(dot(normal, ray.origin) + d) / dotPr;

	if (t < 0) {
		// The intersection is behind the ray origin
		return intersection;
	}

	// Compute the hit point
	CRTVector hitPoint = ray.origin + (ray.direction * t);

	if (pointInTriangle(hitPoint)) {
		intersection.hitPoint = hitPoint;
		intersection.triangleIndex = 0;
		intersection.faceNormal = normal;
		intersection.barycentricCoordinates = getBarycenticCoordinates(hitPoint);
		return intersection;
	}

	return intersection;
}

CRTVector CRTTriangle::getBarycenticCoordinates(const CRTVector& point) const
{
	// we could skip the isInTriangle check to optimize runtime
	CRTTriangle M_triangle(vertices[0], point, vertices[2], normal);	// both subtriangles are from the same plane
	CRTTriangle N_triangle(vertices[0], vertices[1], point, normal);	// so they can use the same normal

	float thisArea = area();

	float u = M_triangle.area() / thisArea;
	float v = N_triangle.area() / thisArea;

	return { u, v, 1 - u - v };
}
