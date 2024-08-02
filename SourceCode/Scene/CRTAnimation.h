#pragma once
#include "CRTScene.h"

static constexpr int FRAMERATE = 24;
static constexpr int DURATION = 120;

static constexpr float ORBIT_PER_FRAME = 5;
static constexpr float FOV_PER_UNIT = 5;

class CRTAnimation
{
	CRTScene* scene;
	bool meshMovement = false;
	int currentKeyframe = 0;
	int framesCount;

	std::vector<CRTCamera> cameraKeyframes;
	std::vector<CRTScene*> meshKeyframes;

	float heightAtDistance(float distance) const;
	float FOVatHeightAndDist(float height, float distance) const;

	std::vector<CRTCamera> generateOrbitKeyframes(const CRTVector& anchor,
		int durationFrames = DURATION, float degreesPerFrame = ORBIT_PER_FRAME, bool snapToAxis = false);
	std::vector<CRTCamera> generateVertigoKeyframes(const float movement, const float focusDistance, int durationFrames = DURATION);
public:
	CRTAnimation(CRTScene* scene);
	const CRTScene* getOriginScene() const;

	static CRTAnimation vertigoAnimation(CRTScene* scene, const float movement, 
		const float focusDistance, int durationFrames = DURATION);
	static CRTAnimation orbitAnimation(CRTScene* scene, const CRTVector& anchor,
		int durationFrames = DURATION, float degreesPerFrame = ORBIT_PER_FRAME, bool snapToAxis = false);

	bool hasNextKeyframe() const;
	int getCurrentFrameIndex() const;
	CRTScene* getNextKeyframe();

};

