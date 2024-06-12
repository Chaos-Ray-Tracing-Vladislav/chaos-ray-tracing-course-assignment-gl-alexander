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

CRTVector& CRTVector::operator+=(const CRTVector& rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

CRTVector& CRTVector::operator-=(const CRTVector& rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

CRTVector& CRTVector::operator*=(float k) {
	x *= k;
	y *= k;
	z *= k;
	return *this;
}

bool CRTVector::operator==(const CRTVector& other) const {
	return abs(this->x - other.x) <= EPSILON
		&& abs(this->y - other.y) <= EPSILON
		&& abs(this->z - other.z) <= EPSILON;
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

CRTVector operator*(const CRTVector& lhs, const float k) {
	CRTVector result = lhs;
	result *= k;
	return result;
}

CRTVector operator*(const float k, const CRTVector& rhs) {
	return rhs * k;
}

/*
Cross product of A, B :
A (xA, yA, zA), B (xB, yB, zB)
is calculated as:
det (	e1, xA, xB
		e2, yA, yB
		e3, zA, zB	)

where e1, e2, e3 are the unit vectors of the space
when calculating it makes out:
*/

CRTVector cross(const CRTVector& A, const CRTVector& B) {
	return CRTVector(	A.y * B.z - A.z * B.y, 
						A.z * B.x - A.x * B.z, 
						A.x * B.y - A.y * B.x	);
}


float dot(const CRTVector& A, const CRTVector& B) {
	return (A.x * B.x) + (A.y * B.y) + (A.z * B.z);
}


