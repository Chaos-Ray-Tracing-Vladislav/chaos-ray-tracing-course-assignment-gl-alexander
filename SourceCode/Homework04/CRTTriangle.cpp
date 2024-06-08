#include "CRTTriangle.h"


CRTTriangle::CRTTriangle(const CRTVector vertices[VERTICES]) {
	for (int i = 0; i < VERTICES; i++) {
		this->vertices[i] = vertices[i];
	}
}

CRTTriangle::CRTTriangle(const CRTVector& v1, const CRTVector& v2, const CRTVector& v3) {
	vertices[0] = v1;
	vertices[1] = v2;
	vertices[2] = v3;
}

/* the order of the vertices matters
		v2


	v0		v1
*/
CRTVector CRTTriangle::normalVector() const {
	CRTVector v1 = vertices[1] - vertices[0];
	CRTVector v2 = vertices[2] - vertices[0];

	return cross(v2, v1); // we return the clockwise cross-product
}


float CRTTriangle::area() const {
	CRTVector v1 = vertices[1] - vertices[0];
	CRTVector v2 = vertices[2] - vertices[0];

	return abs(scalar(v2, v1)) / 2;
}


