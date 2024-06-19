#include "CRTCamera.h"
#include <cmath>

#define PI 3.1415

static const float DEFAULT_ROTATION[N][M] = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
static const float DEFAULT_POSITION[N] = { 0.0f, 0.0f, 0.0f };

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
