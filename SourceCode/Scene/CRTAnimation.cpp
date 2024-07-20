#include "CRTAnimation.h"

CRTAnimation::CRTAnimation(const CRTCamera& cameraOrigin) : cameraOrigin(cameraOrigin)
{
}

std::vector<CRTCamera> CRTAnimation::generateOrbitKeyframes(const CRTVector& anchor, int durationFrames, float degreesPerFrame) const
{
    CRTVector offsetPerFrame = anchor + (cameraOrigin.getPosition() - anchor) * yRotationMatrix(degreesPerFrame) - cameraOrigin.getPosition();
    // because the rotation and movement are applied every frame, the offsetPerFrame vector will be rotated when applied
    // and we need to calculate it only once
    std::vector<CRTCamera> keyframes(durationFrames);
    keyframes[0] = cameraOrigin;
    for (int i = 1; i < durationFrames; i++) {
        keyframes[i] = keyframes[i - 1];
        keyframes[i].moveCamera(offsetPerFrame);
        keyframes[i].pan(degreesPerFrame);
    }
    return keyframes;
}

float CRTAnimation::heightAtDistance(float distance) const
{
    return 2.0f * distance * tan(cameraOrigin.getFOV() / 2 * PI / 180);
}

float CRTAnimation::FOVatHeightAndDist(float height, float distance) const
{
    return 2.0f * atan(height / (2 * distance)) * 180 / PI;
}

std::vector<CRTCamera> CRTAnimation::generateVertigoKeyframes(const float movement, const float focusDistance, int durationFrames) const
{
    float finalFOV = atan(cameraOrigin.getFOVtan() * focusDistance / (focusDistance - movement)) * 2 * 180 / PI;
    //float finalHeight = heightAtDistance(movement);
    //float finalFOV = FOVatHeightAndDist(finalHeight, movement);
    float fovPerFrame = (finalFOV - cameraOrigin.getFOV()) / (durationFrames  - 1);
    float movementPerFrame = movement / (durationFrames - 1);
    std::vector<CRTCamera> keyframes(durationFrames);
    keyframes[0] = cameraOrigin;
    for (int i = 1; i < durationFrames; i++) {
        keyframes[i] = keyframes[i - 1];
        keyframes[i].dolly(movementPerFrame);
        keyframes[i].setFOV(keyframes[i - 1].getFOV() + fovPerFrame);
    }
    return keyframes;
}
