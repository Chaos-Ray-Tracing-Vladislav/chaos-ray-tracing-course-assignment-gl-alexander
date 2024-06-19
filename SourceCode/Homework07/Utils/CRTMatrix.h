#pragma once
#include "CRTVector.h"

const int MATRIX_ROWS = 3;
const int MATRIX_COLUMNS = 3;

class CRTMatrix
{
	float matrix[3][3];
public:
	CRTMatrix();
	CRTMatrix(const float m[MATRIX_ROWS][MATRIX_COLUMNS]);

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
