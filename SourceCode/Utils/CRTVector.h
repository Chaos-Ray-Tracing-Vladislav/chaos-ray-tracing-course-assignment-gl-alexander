#pragma once
#include "Utilities.hpp"

constexpr int AXIS_COUNT = 3;

enum class AxisLabel {
	X = 0,
	Y = 1,
	Z = 2
};

struct CRTVector {
	static const unsigned MEMBERS_COUNT = 3;

	float x;
	float y;
	float z;

	CRTVector();
	CRTVector(float x, float y, float z);
	CRTVector(const float pos[3]);
	
	float length() const;
	float length2() const;
	CRTVector& normalize();
	CRTVector& clamp(float a, float b);

	CRTVector& operator+=(const CRTVector& rhs);
	CRTVector& operator-=(const CRTVector& rhs);

	CRTVector& operator*=(float k);
	CRTVector& operator/=(float k);
	CRTVector& operator*=(const CRTVector& other);

	bool operator==(const CRTVector& other) const;
};

CRTVector operator+(const CRTVector& lhs, const CRTVector& rhs);
CRTVector operator-(const CRTVector& lhs, const CRTVector& rhs);

CRTVector operator-(const CRTVector& unaryVec);

CRTVector operator*(const CRTVector& lhs, float k);
CRTVector operator*(float k, const CRTVector& rhs);
CRTVector operator/(const CRTVector& lhs, float k);
CRTVector operator/(float k, const CRTVector& rhs);
CRTVector operator*(const CRTVector& lhs, const CRTVector& rhs);


CRTVector cross(const CRTVector& lhs, const CRTVector& rhs);
float dot(const CRTVector& lhs, const CRTVector& rhs);
// reflects the incomming ray by the surfaceNormal
CRTVector reflect(const CRTVector& incomming, const CRTVector& surfaceNormal);
// generates a random normal vector in a hemisphere with the passed normal
// param normal: the normal, by which the hemisphere is defined
CRTVector randomHemisphereSample(const CRTVector& normal);
CRTVector randomSphereSample();