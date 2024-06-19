#pragma once
#include "CRTVector.h"

const int N = 3;
const int M = 3;

class CRTMatrix
{
	float matrix[3][3];
public:
	CRTMatrix();
	CRTMatrix(const float m[N][M]);

	CRTMatrix& operator*=(const CRTMatrix& other);
	CRTMatrix& operator+=(const CRTMatrix& other);
	CRTMatrix& operator-=(const CRTMatrix& other);

	friend CRTMatrix operator*(const CRTMatrix& lhs, const CRTMatrix& rhs);
	friend CRTMatrix operator+(const CRTMatrix& lhs, const CRTMatrix& rhs);
	friend CRTMatrix operator-(const CRTMatrix& lhs, const CRTMatrix& rhs);

	friend CRTVector operator*(const CRTVector& lhs, const CRTMatrix& rhs);

	friend CRTMatrix xRotationMatrix(float degrees);
	friend CRTMatrix yRotationMatrix(float degrees);
	friend CRTMatrix zRotationMatrix(float degrees);
};


CRTMatrix xRotationMatrix(float degrees);
CRTMatrix yRotationMatrix(float degrees);
CRTMatrix zRotationMatrix(float degrees);
