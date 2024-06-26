#include "CRTTriangle.h"

CRTTriangle::CRTTriangle(const CRTVector vertices[VERTICES]) : CRTTriangle(vertices, 
	{ MAX_COLOR_COMPONENT, MAX_COLOR_COMPONENT, MAX_COLOR_COMPONENT }) {}


CRTTriangle::CRTTriangle(const CRTVector vertices[VERTICES], const CRTColor& color) {
	for (int i = 0; i < VERTICES; i++) {
		this->vertices[i] = vertices[i];
	}
	calculateNormalVector();
	this->color = color;
}

CRTTriangle::CRTTriangle(const CRTVector& v1, const CRTVector& v2, const CRTVector& v3) : CRTTriangle(v1, v2, v3,
	{ MAX_COLOR_COMPONENT, MAX_COLOR_COMPONENT, MAX_COLOR_COMPONENT }) {}

CRTTriangle::CRTTriangle(const CRTVector& v1, const CRTVector& v2, const CRTVector& v3, const CRTColor& color) {
	vertices[0] = v1;
	vertices[1] = v2;
	vertices[2] = v3;
	calculateNormalVector();
	this->color = color;
}


/* the order of the vertices matters
		v2


	v0		v1
*/
void CRTTriangle::calculateNormalVector(){
	CRTVector v1 = vertices[1] - vertices[0];
	CRTVector v2 = vertices[2] - vertices[0];

	normal = cross(v1, v2); // we set it to the clockwise cross-product
	normal.normalize();
}

const CRTVector& CRTTriangle::getNormal() const {
	return this->normal;
}


const CRTVector* CRTTriangle::getVertices() const {
	return vertices;
}

const CRTColor& CRTTriangle::getColor() const {
	return color;
}


float CRTTriangle::area() const {
	CRTVector v1 = vertices[1] - vertices[0];
	CRTVector v2 = vertices[2] - vertices[0];

	return abs(dot(v2, v1)) / 2;
}

float CRTTriangle::distanceToPoint(const CRTVector& point) const {
	// it doesn't matter which vertex we pick to calculate from
	return dot(normal, point - vertices[0]) / normal.length();
}


bool CRTTriangle::pointInTriangle(const CRTVector& point) const {
	CRTVector E0 = vertices[1] - vertices[0];
	CRTVector E1 = vertices[2] - vertices[1];
	CRTVector E2 = vertices[0] - vertices[2];

	CRTVector relativeE0 = cross(E0, point - vertices[0]);
	CRTVector relativeE1 = cross(E1, point - vertices[1]);
	CRTVector relativeE2 = cross(E2, point - vertices[2]);

	// all the cross products need to be oriented the same way as the normal vector

	return  dot(normal, relativeE0) >= 0 && dot(normal, relativeE1) >= 0 && dot(normal, relativeE2) >= 0;
}
