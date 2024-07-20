#include "CRTVector.h"
#include <cmath>

CRTVector::CRTVector() : CRTVector(0.0, 0.0, 0.0) {}
CRTVector::CRTVector(float x, float y, float z) : x(x), y(y), z(z) {}

CRTVector::CRTVector(const float pos[3])
{
	x = pos[0];
	y = pos[1];
	z = pos[2];
}

float CRTVector::length() const {
	return sqrt(x * x + y * y + z * z);
}

CRTVector& CRTVector::normalize() {
	float originalLength = length();
	float multiplication = 1.0f / length();
	if (std::abs(originalLength - 1) <= EPSILON || originalLength <= EPSILON) {		
		// doesn't normalize already normalized vectors, as well as the 0 length vector
		return *this;
	}
	x *= multiplication;
	y *= multiplication;
	z *= multiplication;
	return *this;
}

CRTVector& CRTVector::clamp(float a, float b)
{
	x = clampValue(x, a, b);
	y = clampValue(y, a, b);
	z = clampValue(z, a, b);
	return *this;
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

CRTVector& CRTVector::operator/=(float k)
{
	return (*this) *= (1.0f / k);
}

// by-component multiplication
CRTVector& CRTVector::operator*=(const CRTVector& other) {
	x *= other.x;
	y *= other.y;
	z *= other.z;
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

CRTVector operator-(const CRTVector& unaryVec)
{
	return unaryVec * -1.0f;
}

CRTVector operator*(const CRTVector& lhs, const float k) {
	CRTVector result = lhs;
	result *= k;
	return result;
}

CRTVector operator*(const float k, const CRTVector& rhs) {
	return rhs * k;
}

CRTVector operator/(const CRTVector& lhs, float k)
{
	CRTVector result = lhs;
	result /= k;
	return result;
}

CRTVector operator/(float k, const CRTVector& rhs)
{
	return rhs / k;
}

// by-component multiplication
CRTVector operator*(const CRTVector& lhs, const CRTVector& rhs) {
	CRTVector result = lhs;
	result *= rhs;
	return result;
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


CRTVector reflect(const CRTVector& incomming, const CRTVector& surfaceNormal)
{
	CRTVector Y = dot(incomming, surfaceNormal) * surfaceNormal;
	return (incomming - 2 * Y).normalize();
}

CRTVector randomHemisphereSample(const CRTVector& normal)
{
	double u = randFloat();
	double v = randFloat();

	double theta = 2 * PI * u;
	double cosPhi = 2 * v - 1;
	double sinPhi = sqrt(1 - cosPhi * cosPhi);

	CRTVector vec(
		cos(theta) * sinPhi,
		cosPhi,
		sin(theta) * sinPhi
	);

	/// restrict to the hemisphere: it is on the wrong side, just flip the result:
	if (dot(vec, normal) < 0)
		vec = -vec;

	return vec;
}


