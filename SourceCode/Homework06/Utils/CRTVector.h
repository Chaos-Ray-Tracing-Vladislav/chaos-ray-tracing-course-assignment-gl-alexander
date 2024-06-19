#pragma once

#define EPSILON 0.00001

struct CRTVector
{
	float x;
	float y;
	float z;

	CRTVector();
	CRTVector(float x, float y, float z);
	CRTVector(const float pos[3]);
	
	float length() const;
	void normalize();

	CRTVector& operator+=(const CRTVector& rhs);
	CRTVector& operator-=(const CRTVector& rhs);

	CRTVector& operator*=(float k);

	bool operator==(const CRTVector& other) const;
};

CRTVector operator+(const CRTVector& lhs, const CRTVector& rhs);
CRTVector operator-(const CRTVector& lhs, const CRTVector& rhs);

CRTVector operator*(const CRTVector& lhs, float k);
CRTVector operator*(float k, const CRTVector& lhs);


CRTVector cross(const CRTVector& lhs, const CRTVector& rhs);
float dot(const CRTVector& lhs, const CRTVector& rhs);


