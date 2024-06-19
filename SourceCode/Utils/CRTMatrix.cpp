#include "CRTMatrix.h"
#include <cmath>

const float PI = 3.1415;

CRTMatrix::CRTMatrix(const float matrix[MATRIX_ROWS][MATRIX_COLUMNS]) {
	for (int i = 0; i < MATRIX_ROWS; i++) {
		for (int j = 0; j < MATRIX_COLUMNS; j++) {
			this->matrix[i][j] = matrix[i][j];
		}
	}
}

CRTMatrix::CRTMatrix() {
	for (int i = 0; i < MATRIX_ROWS; i++) {
		for (int j = 0; j < MATRIX_COLUMNS; j++) {
			this->matrix[i][j] = 0;
		}
	}
}

CRTMatrix& CRTMatrix::operator*=(const CRTMatrix& other) {
	*this = (*this) * other;
	return *this;
}
CRTMatrix& CRTMatrix::operator+=(const CRTMatrix& other) {
	for (int i = 0; i < MATRIX_ROWS; ++i) {
		for (int j = 0; j < MATRIX_COLUMNS; ++j) {
			matrix[i][j] += other.matrix[i][j];
		}
	}
	return *this;
}
CRTMatrix& CRTMatrix::operator-=(const CRTMatrix& other) {
	for (int i = 0; i < MATRIX_ROWS; ++i) {
		for (int j = 0; j < MATRIX_COLUMNS; ++j) {
			matrix[i][j] -= other.matrix[i][j];
		}
	}
	return *this;
}

CRTMatrix operator*(const CRTMatrix& lhs, const CRTMatrix& rhs) {
	CRTMatrix result;
	for (int i = 0; i < MATRIX_ROWS; ++i) {
		for (int j = 0; j < MATRIX_ROWS; ++j) {
			result.matrix[i][j] = 0.0f;
			for (int k = 0; k < MATRIX_ROWS; ++k) {
				result.matrix[i][j] += lhs.matrix[i][k] * rhs.matrix[k][j];
			}
		}
	}
	return result;
}
CRTMatrix operator+(const CRTMatrix& lhs, const CRTMatrix& rhs) {
	CRTMatrix result = lhs;
	result += rhs;
	return result;
}
CRTMatrix operator-(const CRTMatrix& lhs, const CRTMatrix& rhs) {
	CRTMatrix result = lhs;
	result += rhs;
	return result;
}

CRTVector operator*(const CRTVector& lhs, const CRTMatrix& rhs) {
	// row-major multiplication
	CRTVector result;
	result.x += lhs.x * rhs.matrix[0][0];
	result.x += lhs.y * rhs.matrix[0][1];
	result.x += lhs.z * rhs.matrix[0][2];

	result.y += lhs.x * rhs.matrix[1][0];
	result.y += lhs.y * rhs.matrix[1][1];
	result.y += lhs.z * rhs.matrix[1][2];

	result.z += lhs.x * rhs.matrix[2][0];
	result.z += lhs.y * rhs.matrix[2][1];
	result.z += lhs.z * rhs.matrix[2][2];
	return result;
}


CRTMatrix xRotationMatrix(float degrees) {
	// counterclockwise on the X axis
	float rad = degrees * PI / 180.0f;
	float cosA = cos(rad);
	float sinA = sin(rad);

	float rotX[3][3] = {
			{1.0f, 0.0f, 0.0f},
			{0.0f, cosA, -sinA},
			{0.0f, sinA, cosA}
	};

	return CRTMatrix(rotX);
}

CRTMatrix yRotationMatrix(float degrees) {
	// rotate counterclockwise on the Y axis
	float rad = (degrees * PI) / 180.0f;
	float cosA = cos(rad);
	float sinA = sin(rad);

	float rotY[3][3] = {
	{cosA, 0.0f, sinA},
	{0.0f, 1.0f, 0.0f},
	{-sinA, 0.0f, cosA}
	};

	return CRTMatrix(rotY);
}

CRTMatrix zRotationMatrix(float degrees) {
	// counterclockwise on the Z axis
	float rad = degrees * PI / 180.0f;
	float cosA = cos(rad);
	float sinA = sin(rad);

	float rotZ[3][3] = {
	{cosA, -sinA, 0.0f},
	{sinA, cosA, 0.0f},
	{0.0f, 0.0f, 1.0f}
	};

	return CRTMatrix(rotZ);
}
