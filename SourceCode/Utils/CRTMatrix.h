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
	CRTMatrix(CRTVector e[MATRIX_ROWS]);
	CRTMatrix(const CRTVector& e1, const CRTVector& e2, const CRTVector& e3);

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

	CRTMatrix transpose() const;
};


CRTMatrix xRotationMatrix(float degrees);
CRTMatrix yRotationMatrix(float degrees);
CRTMatrix zRotationMatrix(float degrees);
