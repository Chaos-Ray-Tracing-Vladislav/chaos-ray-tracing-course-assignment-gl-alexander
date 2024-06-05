#include "CRTVector.h"
#include <cmath>

CRTVector::CRTVector() : CRTVector(0.0, 0.0, 0.0) {}
CRTVector::CRTVector(float x, float y, float z) : x(x), y(y), z(z) {}

float CRTVector::length() const {
	return sqrt(x * x + y * y + z * z);
}

void CRTVector::normalize() {
	float originalLength = length();
	if (originalLength <= EPSILON) {
		return;
	}
	x /= originalLength;
	y /= originalLength;
	z /= originalLength;
}

void CRTVector::operator+=(const CRTVector& rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
}

void CRTVector::operator-=(const CRTVector& rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
}

CRTVector operator+(const CRTVector& lhs, const CRTVector& rhs) {
	CRTVector result = lhs;
	result += rhs;
	return result;
}

CRTVector operator-(const CRTVector& lhs, const CRTVector& rhs) {
	CRTVector result = lhs;
	result -= rhs;
	return result;
}