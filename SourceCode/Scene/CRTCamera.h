#pragma once
#include <vector>
#include "CRTMesh.h"
#include "../Utils/CRTVector.h"
#include "../Utils/CRTMatrix.h"
#include "../Utils/CRTRay.h"

static const float DEFAULT_FOV = 90;

class CRTCamera
{
	CRTVector position;
	CRTVector frontDirection, rightDirection, upDirection;
	CRTMatrix rotation;
	CRTMesh imagePlane;
	
	unsigned imageWidth;
	unsigned imageHeight;
	float FOV;
	float tanFOV;

	CRTRay getRayForSubpixel(float rowId, float colId, const CRTVector& position) const;
public:
	CRTCamera();
	CRTCamera(const CRTVector& position, float FOV = DEFAULT_FOV);
	CRTCamera(const CRTVector& position, const CRTMatrix& rotation, float FOV = DEFAULT_FOV);

	const CRTVector& getPosition() const;

	const CRTVector& getFrontDirection() const;
	const CRTVector& getUpDirection() const;
	const CRTVector& getRightDirection() const;

	const CRTMatrix& getRotation() const;

	void setPosition(const CRTVector& position);
	void setRotation(const CRTMatrix& matrix);
	void setImageSettings(unsigned width, unsigned height);

	CRTRay getRayForPixel(unsigned rowId, unsigned colId) const;
	CRTRay getRayForSubpixel(float rowId, float colId) const;
	std::pair<int, int> getRayHitpoint(const CRTRay& ray) const;

	std::pair<CRTRay, CRTRay> getEyeRays(float rowId, float colId, float eyeDistance) const;

	void moveCamera(const CRTVector& position);
	void dolly(float distance);
	void truck(float distance);
	void pedestal(float distance);

	void pan(float degrees);
	void tilt(float degrees);
	void roll(float degrees);

	void setFOV(float FOV);
	float getFOV() const;
	float getFOVtan() const;

	unsigned getWidth() const;
	unsigned getHeight() const;

	void updateDirections();
};

