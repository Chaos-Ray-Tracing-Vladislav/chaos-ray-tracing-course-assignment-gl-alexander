#pragma once
constexpr float EPSILON = 0.00001;
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
	CRTVector& normalize();

	CRTVector& operator+=(const CRTVector& rhs);
	CRTVector& operator-=(const CRTVector& rhs);

	CRTVector& operator*=(float k);
	CRTVector& operator*=(const CRTVector& other);

	bool operator==(const CRTVector& other) const;
};

CRTVector operator+(const CRTVector& lhs, const CRTVector& rhs);
CRTVector operator-(const CRTVector& lhs, const CRTVector& rhs);

CRTVector operator-(const CRTVector& unaryVec);

CRTVector operator*(const CRTVector& lhs, float k);
CRTVector operator*(float k, const CRTVector& rhs);
CRTVector operator*(const CRTVector& lhs, const CRTVector& rhs);


CRTVector cross(const CRTVector& lhs, const CRTVector& rhs);
float dot(const CRTVector& lhs, const CRTVector& rhs);
CRTVector reflect(const CRTVector& incomming, const CRTVector& surfaceNormal);