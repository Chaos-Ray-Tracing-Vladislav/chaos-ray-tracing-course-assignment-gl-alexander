#include "CRTAnimation.h"

CRTAnimation::CRTAnimation(CRTScene* scene) : scene(scene)
{
}

const CRTScene* CRTAnimation::getOriginScene() const
{
    return scene;
}

CRTAnimation CRTAnimation::vertigoAnimation(CRTScene* scene, const float movement, const float focusDistance, int durationFrames)
{
    CRTAnimation anim(scene);
    anim.meshMovement = false;
    anim.framesCount = durationFrames;
    anim.generateVertigoKeyframes(movement, focusDistance, durationFrames);
}

CRTAnimation CRTAnimation::orbitAnimation(CRTScene* scene, const CRTVector& anchor, int durationFrames, float degreesPerFrame, bool snapToAxis)
{
    CRTAnimation anim(scene);
    anim.meshMovement = false;
    anim.framesCount = durationFrames;
    anim.generateOrbitKeyframes(anchor, durationFrames, degreesPerFrame, snapToAxis);
}

bool CRTAnimation::hasNextKeyframe() const
{
    return currentKeyframe < framesCount;
}

int CRTAnimation::getCurrentFrameIndex() const
{
    return currentKeyframe;
}

CRTScene* CRTAnimation::getNextKeyframe()
{
    if (meshMovement) {
        return meshKeyframes[currentKeyframe++];
    }
    else { // this means we don't have to parse the objects and rebuild the KD tree
        scene->setCamera(cameraKeyframes[currentKeyframe++]);
        return scene;
    }
    return nullptr;
}

std::vector<CRTCamera> CRTAnimation::generateOrbitKeyframes(const CRTVector& anchorPoint,
    int durationFrames, float degreesPerFrame, bool snapToAxis) 
{
    CRTVector anchor = anchorPoint;
    if (snapToAxis) {
        CRTVector absOffset = anchor - scene->getCamera().getPosition();
        absOffset.x = abs(absOffset.x);
        absOffset.y = abs(absOffset.y);
        absOffset.z = abs(absOffset.z);
        if (absOffset.x < absOffset.y) {
            if (absOffset.x < absOffset.z) {
                anchor.x = scene->getCamera().getPosition().x;
            }
            else {
                anchor.z = scene->getCamera().getPosition().z;
            }
        }
        else {
            if (absOffset.y < absOffset.z) {
                anchor.y = scene->getCamera().getPosition().y;
            }
            else {
                anchor.z = scene->getCamera().getPosition().z;
            }
        }
    }

    CRTVector offsetPerFrame = anchor + (scene->getCamera().getPosition() - anchor) * yRotationMatrix(degreesPerFrame) 
        - scene->getCamera().getPosition();
    // because the rotation and movement are applied every frame, the offsetPerFrame vector will be rotated when applied
    // and we need to calculate it only once
    cameraKeyframes.resize(durationFrames);
    cameraKeyframes[0] = scene->getCamera();
    for (int i = 1; i < durationFrames; i++) {
        cameraKeyframes[i] = cameraKeyframes[i - 1];
        cameraKeyframes[i].moveCamera(offsetPerFrame);
        cameraKeyframes[i].pan(degreesPerFrame);
    }
    meshMovement = false;
    return cameraKeyframes;
}

float CRTAnimation::heightAtDistance(float distance) const
{
    return 2.0f * distance * tan(scene->getCamera().getFOV() / 2 * PI / 180);
}

float CRTAnimation::FOVatHeightAndDist(float height, float distance) const
{
    return 2.0f * atan(height / (2 * distance)) * 180 / PI;
}

std::vector<CRTCamera> CRTAnimation::generateVertigoKeyframes(const float movement, const float focusDistance, int durationFrames)
{
    float finalFOV = atan(scene->getCamera().getFOVtan() * focusDistance / (focusDistance - movement)) * 2 * 180 / PI;
    float fovPerFrame = (finalFOV - scene->getCamera().getFOV()) / (durationFrames  - 1);
    float movementPerFrame = movement / (durationFrames - 1);
    cameraKeyframes.resize(durationFrames);
    cameraKeyframes[0] = scene->getCamera();
    for (int i = 1; i < durationFrames; i++) {
        cameraKeyframes[i] = cameraKeyframes[i - 1];
        cameraKeyframes[i].dolly(movementPerFrame);
        cameraKeyframes[i].setFOV(cameraKeyframes[i - 1].getFOV() + fovPerFrame);
    }
    meshMovement = false;
    return cameraKeyframes;
}
