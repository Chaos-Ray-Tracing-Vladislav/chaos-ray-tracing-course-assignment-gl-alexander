#pragma once
#include "Utils/CRTMatrix.h"
#include "Utils/CRTVector.h"

class CRTCamera
{
	CRTVector position;
	CRTMatrix rotation;
public:
	CRTCamera();
	CRTCamera(const CRTVector& position);
	CRTCamera(const CRTVector& position, const CRTMatrix& rotation);

	const CRTVector& getPosition() const;
	const CRTMatrix& getRotation() const;

	void moveCamera(const CRTVector& position);
	void dolly(float distance);
	void truck(float distance);
	void pedestal(float distance);

	void pan(float degrees);
	void tilt(float degrees);
	void roll(float degrees);
};

