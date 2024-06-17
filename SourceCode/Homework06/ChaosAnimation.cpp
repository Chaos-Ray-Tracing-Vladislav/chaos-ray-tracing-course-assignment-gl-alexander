#include <iostream>
#include <string>
#include "Utils/CRTImageSaver.h"
#include "Utils/CRTTriangle.h"
#include "CRTRenderer.h"
#include "CRTScreen.h"

class CRTPyramid {
public:
    std::vector<CRTTriangle> faces;

    CRTPyramid(const CRTVector& baseCenter, float baseSize, float height) {
        CRTVector apex(baseCenter.x, baseCenter.y + height, baseCenter.z);

        CRTVector v0(baseCenter.x - baseSize / 2, baseCenter.y, baseCenter.z - baseSize / 2);
        CRTVector v1(baseCenter.x + baseSize / 2, baseCenter.y, baseCenter.z - baseSize / 2);
        CRTVector v2(baseCenter.x + baseSize / 2, baseCenter.y, baseCenter.z + baseSize / 2);
        CRTVector v3(baseCenter.x - baseSize / 2, baseCenter.y, baseCenter.z + baseSize / 2);

        faces.push_back(CRTTriangle(v0, v1, apex, { 255,255,186 })); // yellow 
        faces.push_back(CRTTriangle(v1, v2, apex, { 186,255,201 })); // green
        faces.push_back(CRTTriangle(v2, v3, apex, { 255,179,186 })); // red
        faces.push_back(CRTTriangle(v3, v0, apex, { 186,225,255 })); // light blue

        faces.push_back(CRTTriangle(v1, v0, v2));
        faces.push_back(CRTTriangle(v2, v0, v3));
    }
};


CRTScene generatePyramid() {
    CRTPyramid pi(CRTVector(0, 0, -3), 1, 1.5);
    return pi.faces;
}

CRTScene generateTetrahedron() {
    // Define vertices for the pyramid
    CRTVector v0(0, 0, -3);
    CRTVector v1(1, 0, -3);
    CRTVector v2(0.5, 1, -3);
    CRTVector apex(0.5, 0.5, -2);

    // Create triangles
    std::vector<CRTTriangle> pyramid;
    pyramid.push_back(CRTTriangle(apex, v0, v1, { 255, 0, 0 }));  // Base triangle
    pyramid.push_back(CRTTriangle(apex, v1, v2, { 0 , 255, 0 }));  // Side triangle 1
    pyramid.push_back(CRTTriangle(apex, v2, v0, { 0, 0, 255 }));  // Side triangle 2
    pyramid.push_back(CRTTriangle(v1, v0, v2, { 255, 255, 255 }));  //bottom triangle
    return pyramid;
}

void task1() {
    CRTCamera camera;
    camera.pedestal(-1);
    camera.truck(1);
    // camera position should now be {1, -1, 0}
    CRTScreen screen(camera, 800, 800); // makes a 800 by 800 screen offset by 1 on the z axis

    CRTRenderer render(screen);

    CRTTriangle tri(
        CRTVector(-1.75, -1.75, -3),
        CRTVector(1.75, -1.75, -3),
        CRTVector(0, 1.75, -3),
        CRTColor(255, 255, 0));
    CRTScene simpleScene;
    simpleScene.push_back(tri);
    CRTImage result = render.renderScene(simpleScene, visualizeByTriangle);
    CRTImageSaver::saveImage("cameraOffset.ppm", result);
}

void task2_before_movement() {
    CRTCamera camera;
    CRTScreen screen(camera, 800, 800);

    CRTRenderer render(screen);

    CRTScene pyramid = generateTetrahedron();

    CRTImage result = render.renderScene(pyramid, visualizeByTriangle);
    CRTImageSaver::saveImage("pyramidBeforeMovements.ppm", result);
}

void task2_after_movement() {
    CRTCamera camera;
    camera.truck(-0.5);
    camera.pedestal(2);
    camera.pan(-5);
    CRTScreen screen(camera, 800, 800); 

    CRTRenderer render(screen);

    CRTScene pyramid = generateTetrahedron();

    CRTImage result = render.renderScene(pyramid, visualizeByTriangle);
    CRTImageSaver::saveImage("pyramidWithMovement.ppm", result);
}

void task5() {
    CRTScene pyramid = generateTetrahedron();
    CRTCamera camera;

    int frames = 72;
    double anglePerFrame = 5;

    CRTVector anchorPoint(0, 0, -3); // works well for this pyramid
    CRTVector offsetPerFrame = anchorPoint + (camera.getPosition() - anchorPoint) * yRotationMatrix(anglePerFrame) - camera.getPosition();
    // because the rotation and movement are applied every frame, the offsetPerFrame vector will be rotated when applied
    // and we need to calculate it only once
    for (int i = 0; i < frames; i++) {
        char filename[128];
        sprintf_s(filename, "animation/frame_%02d.ppm", i);
        camera.moveCamera(offsetPerFrame);
        camera.pan(anglePerFrame);
        CRTScreen screen(camera, 300, 300); // 300x300 so render time is lower

        CRTRenderer render(screen);
        CRTImage result = render.renderScene(pyramid, visualizeByTriangle);
        CRTImageSaver::saveImage(filename, result);
    }
}

int main()
{
    task5();
}
