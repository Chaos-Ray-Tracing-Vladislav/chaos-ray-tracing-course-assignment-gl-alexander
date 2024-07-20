#pragma once
#include "CRTCamera.h"

static constexpr int FRAMERATE = 24;
static constexpr int DURATION = 120;

static constexpr float ORBIT_PER_FRAME = 5;
static constexpr float FOV_PER_UNIT = 5;

class CRTAnimation
{
	const CRTCamera& cameraOrigin;
	float heightAtDistance(float distance) const;
	float FOVatHeightAndDist(float height, float distance) const;
public:
	CRTAnimation(const CRTCamera& cameraOrigin);

	std::vector<CRTCamera> generateOrbitKeyframes(const CRTVector& anchor, 
		int durationFrames = DURATION, float degreesPerFrame = ORBIT_PER_FRAME) const;
	std::vector<CRTCamera> generateVertigoKeyframes(const float movement, const float focusDistance, int durationFrames = DURATION) const;
};

