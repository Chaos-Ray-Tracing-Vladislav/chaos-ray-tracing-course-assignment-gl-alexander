#include "CRTCamera.h"
#include <cmath>
#include <vector>

#define PI 3.1415

static const CRTVector cameraOffsetFromImage(0, 0, -1);
static const float PIXEL_LENGTH = 1.0f;
static const float DEFAULT_ROTATION[MATRIX_ROWS][MATRIX_COLUMNS] = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
static const float DEFAULT_POSITION[MATRIX_ROWS] = { 0.0f, 0.0f, 0.0f };

CRTCamera::CRTCamera() : CRTCamera(DEFAULT_POSITION, DEFAULT_ROTATION)
{}

CRTCamera::CRTCamera(const CRTVector& position) : CRTCamera(position, DEFAULT_ROTATION)
{}

CRTCamera::CRTCamera(const CRTVector& position, const CRTMatrix& rotation) : position(position), rotation(rotation)
{}

const CRTVector& CRTCamera::getPosition() const {
	return position;
}

const CRTMatrix& CRTCamera::getRotation() const {
	return rotation;
}

void CRTCamera::setPosition(const CRTVector& position)
{
	this->position = position;
}

void CRTCamera::setRotation(const CRTMatrix& matrix)
{
	this->rotation = matrix;
}

void CRTCamera::setImageSettings(unsigned width, unsigned height)
{
	imageWidth = width;
	imageHeight = height;
}

CRTRay CRTCamera::getRayForPixel(unsigned rowId, unsigned colId) const {
	float x_coordinate = static_cast<float>(colId);
	float y_coordinate = static_cast<float>(rowId);
	float z_coordinate = 0;		// relative to the screen, its z is 0

	// Algorithm from https://github.com/Chaos-Ray-Tracing-Vladislav/Homework/blob/main/HomeworkTasks/03%20Rays/CRT%20Homework%2003%20Rays.pdf
	// Convert to NDC space
	x_coordinate /= imageWidth;
	y_coordinate /= imageHeight;

	// Convert NDC coordiantes to Screen space 
	x_coordinate = (2.0 * x_coordinate) - 1.0;
	y_coordinate = 1.0 - (2.0 * y_coordinate);

	// Consider the aspect ratio
	x_coordinate *= (float)imageWidth / (float)imageHeight;

	// Account for the desired pixel length
	x_coordinate *= PIXEL_LENGTH;
	y_coordinate *= PIXEL_LENGTH;

	// Convert to World space coordinates based on screen position
	CRTVector direction(x_coordinate, y_coordinate, z_coordinate);
	direction += cameraOffsetFromImage; // since vec.-matrix multiplication is distributive, so we can add the offset to the pixel direction
	// and rotate it afterwards
	direction = direction * this->rotation;
	direction.normalize();

	return CRTRay(this->position, direction);
}

void CRTCamera::moveCamera(const CRTVector& position) {
	CRTVector adjustedPosition = position * rotation;
	this->position += adjustedPosition;
}

void CRTCamera::dolly(float distance) {
	// on the Z axis
	CRTVector toMove(0, 0, -distance);
	moveCamera(toMove);
}

void CRTCamera::truck(float distance) {
	// on the X axis
	CRTVector toMove(distance, 0, 0);
	moveCamera(toMove);
}

void CRTCamera::pedestal(float distance) {
	// on the Y axis
	CRTVector toMove(0, distance, 0);
	moveCamera(toMove);
}

void CRTCamera::pan(float degrees) {
	// rotate counterclockwise on the Y axis
	CRTMatrix rotMatrix = yRotationMatrix(degrees);
	rotation = rotMatrix * rotation;
}

void CRTCamera::tilt(float degrees) {
	// counterclockwise on the X axis
	CRTMatrix rotMatrix = xRotationMatrix(degrees);
	rotation = rotMatrix * rotation;
}

void CRTCamera::roll(float degrees) {
	// counterclockwise on the Z axis
	CRTMatrix rotMatrix = zRotationMatrix(degrees);
	rotation = rotMatrix * rotation;
}


