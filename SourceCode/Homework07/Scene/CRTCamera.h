#pragma once
#include <vector>
#include "../Utils/CRTMatrix.h"
#include "../Utils/CRTVector.h"
#include "../Utils/CRTRay.h"

using CRTRayMatrix = std::vector<std::vector<CRTRay>>;


class CRTCamera
{
	CRTVector position;
	CRTMatrix rotation;

	unsigned imageWidth;
	unsigned imageHeight;

public:
	CRTCamera();
	CRTCamera(const CRTVector& position);
	CRTCamera(const CRTVector& position, const CRTMatrix& rotation);

	const CRTVector& getPosition() const;
	const CRTMatrix& getRotation() const;

	void setPosition(const CRTVector& position);
	void setRotation(const CRTMatrix& matrix);
	void setImageSettings(unsigned width, unsigned height);

	CRTRay getRayForPixel(unsigned rowId, unsigned colId) const;

	void moveCamera(const CRTVector& position);
	void dolly(float distance);
	void truck(float distance);
	void pedestal(float distance);

	void pan(float degrees);
	void tilt(float degrees);
	void roll(float degrees);
};

