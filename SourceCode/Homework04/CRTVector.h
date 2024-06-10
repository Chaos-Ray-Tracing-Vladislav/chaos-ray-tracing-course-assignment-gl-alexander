#pragma once

#define EPSILON 0.00001

class CRTVector
{
public:
	float x;
	float y;
	float z;

	CRTVector();
	CRTVector(float x, float y, float z);
	
	float length() const;
	void normalize();

	void operator+=(const CRTVector& rhs);
	void operator-=(const CRTVector& rhs);

	friend CRTVector operator+(const CRTVector& lhs, const CRTVector& rhs);
	friend CRTVector operator-(const CRTVector& lhs, const CRTVector& rhs);

	friend CRTVector cross(const CRTVector& lhs, const CRTVector& rhs);
	friend float scalar(const CRTVector& lhs, const CRTVector& rhs);
};

CRTVector operator+(const CRTVector& lhs, const CRTVector& rhs);
CRTVector operator-(const CRTVector& lhs, const CRTVector& rhs);

CRTVector cross(const CRTVector& lhs, const CRTVector& rhs);
float scalar(const CRTVector& lhs, const CRTVector& rhs);


