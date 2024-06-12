#include <iostream>
#include "Utils/CRTScreen.h"
#include "Utils/CRTImageSaver.h"
#include "Utils/CRTTriangle.h"
#include "CRTRenderer.h"



CRTColor visualizeByDepth(const CRTVector& point, const CRTTriangle& triangle, float maxDistance) {
    short colorValue = (1 - (point.length() / (maxDistance * 2))) * MAX_COLOR_COMPONENT;
    return { colorValue, colorValue, colorValue };
}

CRTColor visualizeByTriangle(const CRTVector& point, const CRTTriangle& triangle, float maxDistance) {
    float colorPercentage = point.length() / (maxDistance * 1.5);
    CRTColor result = triangle.getColor();
    result.r *= colorPercentage;
    result.g *= colorPercentage;
    result.b *= colorPercentage;

    return result;
}

int main()
{
    CRTVector cameraPosition(0.0, 0.0, 0.0);
    CRTVector screenPosition(0.0, 0.0, -1.0);
    CRTScreen screen(screenPosition, 800, 800);

    CRTRenderer render(screen, cameraPosition);

    CRTScene pyramid;

    // Define the vertices of the pyramid
    CRTVector apex(0, 2, -5); // Apex of the pyramid
    CRTVector base1(-2, -1, -6); // Skewed bottom-left vertex of the base
    CRTVector base2(2, -2, -6); // Skewed bottom-right vertex of the base
    CRTVector base3(1, 1, -4); // Skewed top-right vertex of the base
    CRTVector base4(-1, 2, -4); // Skewed top-left vertex of the base

    // Create triangles from the vertices in reverse order
    // Base of the pyramid (two triangles to form a skewed quadrilateral)
    pyramid.push_back(CRTTriangle(base3, base2, base1));
    pyramid.push_back(CRTTriangle(base4, base3, base1));

    // Sides of the pyramid
    pyramid.push_back(CRTTriangle(base1, base2, apex, { 255, 0, 0 })); // Front face, red
    pyramid.push_back(CRTTriangle(base2, base3, apex, { 0 , 255, 0 })); // Right face green
    pyramid.push_back(CRTTriangle(base3, base4, apex, { 0, 0, 255 })); // Back face, blue
    pyramid.push_back(CRTTriangle(base4, base1, apex, { 255, 0, 255 })); // Left face

    CRTImage result = render.renderScene(pyramid, visualizeByTriangle);
    CRTImageSaver::saveImage("pyramid.ppm", result);
}
