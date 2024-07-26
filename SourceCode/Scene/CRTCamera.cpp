#include "CRTCamera.h"
#include <cmath>
#include <vector>

static const CRTVector FRONT(0, 0, -1);
static const CRTVector UP(0, 1, 0);
static const CRTVector RIGHT(1, 0, 0);
static const float DEFAULT_ROTATION[MATRIX_ROWS][MATRIX_COLUMNS] = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
static const float DEFAULT_POSITION[MATRIX_ROWS] = { 0.0f, 0.0f, 0.0f };


CRTCamera::CRTCamera() : CRTCamera(DEFAULT_POSITION, DEFAULT_ROTATION)
{}

CRTCamera::CRTCamera(const CRTVector& position, float FOV) : CRTCamera(position, DEFAULT_ROTATION, FOV)
{}

CRTCamera::CRTCamera(const CRTVector& position, const CRTMatrix& rotation, float FOV) : position(position), rotation(rotation), FOV(FOV)
{
	tanFOV = tan((FOV / 2) * PI / 180.0);
}

const CRTVector& CRTCamera::getPosition() const {
	return position;
}

const CRTVector& CRTCamera::getFrontDirection() const
{
	return frontDirection;
}

const CRTVector& CRTCamera::getUpDirection() const
{
	return upDirection;
}

const CRTVector& CRTCamera::getRightDirection() const
{
	return rightDirection;
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

CRTRay CRTCamera::getRayForSubpixel(float rowId, float colId, const CRTVector& position) const
{
	float x_coordinate = colId;
	float y_coordinate = rowId;
	float z_coordinate = 0;		// relative to the screen, its z is 0

	// Convert to NDC space
	x_coordinate /= imageWidth;
	y_coordinate /= imageHeight;

	// Convert NDC coordiantes to Screen space 
	x_coordinate = (2.0 * x_coordinate) - 1.0;
	y_coordinate = 1.0 - (2.0 * y_coordinate);

	// Consider the aspect ratio
	x_coordinate *= (float)imageWidth / (float)imageHeight;

	// Account for FOV
	x_coordinate *= tanFOV;
	y_coordinate *= tanFOV;

	// Convert to World space coordinates based on screen position
	CRTVector direction(x_coordinate, y_coordinate, z_coordinate);
	direction += FRONT; // since vec.-matrix multiplication is distributive, so we can add the offset to the pixel direction
	// and rotate it afterwards
	direction = direction * this->rotation;
	direction.normalize();

	return {position, direction, RayType::CAMERA, 0 };
}

void CRTCamera::updateDirections()
{
	frontDirection = FRONT * rotation;
	upDirection = UP * rotation;
	rightDirection = RIGHT * rotation;

	CRTVector lowerLeftCorner = getRayForPixel(imageHeight - 1, 0).direction + position;
	CRTVector lowerRightCorner = getRayForPixel(imageHeight - 1, imageWidth - 1).direction + position;
	CRTVector upperRightCorner = getRayForPixel(0, imageWidth -1).direction + position;
	CRTVector upperLeftCorner = getRayForPixel(0, 0).direction + position;

	imagePlane = CRTMesh(
		std::vector<CRTVector>({ lowerLeftCorner, lowerRightCorner, upperLeftCorner, upperRightCorner }),
		std::vector<int>({ 0,1,2,1,3,2 }),
		0);
}

CRTRay CRTCamera::getRayForPixel(unsigned rowId, unsigned colId) const {
	return getRayForSubpixel(rowId, colId); // the default ray per pixel gives us a pixel shooting at the upper left corner of the pixel
}

CRTRay CRTCamera::getRayForSubpixel(float rowId, float colId) const
{
	return getRayForSubpixel(rowId, colId, this->position);
}

std::pair<int, int> CRTCamera::getRayHitpoint(const CRTRay& ray) const
{
	Intersection data = imagePlane.intersectsRay(ray);
	if (data.triangleIndex == NO_HIT_INDEX) {
		return { -1, -1 };
	}
	CRTVector uvCoords = imagePlane.getUV(data);
	int pixelX = uvCoords.x * imageWidth;
	int pixelY = (1 - uvCoords.y) * imageHeight;
	return { pixelX, pixelY };
}

std::pair<CRTRay, CRTRay> CRTCamera::getEyeRays(float rowId, float colId, float eyeDistance) const
{
	float halfdistance = eyeDistance / 2;
	CRTVector leftEyePos = this->position - rightDirection * halfdistance;
	CRTVector rightEyePos = this->position + rightDirection * halfdistance;
	return { getRayForSubpixel(rowId, colId, leftEyePos), getRayForSubpixel(rowId, colId, rightEyePos) };
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
	updateDirections();
}

void CRTCamera::tilt(float degrees) {
	// counterclockwise on the X axis
	CRTMatrix rotMatrix = xRotationMatrix(degrees);
	rotation = rotMatrix * rotation;
	updateDirections();
}

void CRTCamera::roll(float degrees) {
	// counterclockwise on the Z axis
	CRTMatrix rotMatrix = zRotationMatrix(degrees);
	rotation = rotMatrix * rotation;
	updateDirections();
}

void CRTCamera::setFOV(float FOV)
{
	this->FOV = FOV;
	this->tanFOV = tan((FOV / 2) * PI / 180.0);
}

float CRTCamera::getFOV() const
{
	return FOV;
}

float CRTCamera::getFOVtan() const
{
	return tanFOV;
}

unsigned CRTCamera::getWidth() const
{
	return imageWidth;
}

unsigned CRTCamera::getHeight() const
{
	return imageHeight;
}


