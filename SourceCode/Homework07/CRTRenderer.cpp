#include "CRTRenderer.h"
#include <algorithm>
#include <iostream>
#include <climits>


std::vector<CRTTriangle> CRTRenderer::convertMeshesToTriangles() const
{
    std::vector<CRTTriangle> allTriangles;
    for (int i = 0; i < scene->getGeometryObjects().size(); i++) {
        std::vector<CRTTriangle> currentMeshTriangles = scene->getGeometryObjects()[i].getTriangles();

        allTriangles.insert(allTriangles.end(), currentMeshTriangles.begin(), currentMeshTriangles.end());
    }
    return allTriangles;
}

CRTRenderer::CRTRenderer(const CRTScene* scene) : scene(scene)
{}


void CRTRenderer::renderScene(const char* outputname) const
{
    CRTImage img = renderScene();
    CRTImageSaver::saveImage(outputname, img);
}

CRTImage CRTRenderer::renderScene() const {
    return renderScene(visualizeByDepth);
}

float CRTRenderer::getMaxDistanceToCamera(const std::vector<CRTTriangle>& triangles) const {
    float maxDistance = -1;
    for (int i = 0; i < triangles.size(); i++) {
        maxDistance = std::max(maxDistance, std::abs(triangles[i].distanceToPoint(scene->getCamera().getPosition())));
    }
    return maxDistance;
}

CRTImage CRTRenderer::renderScene(std::function<CRTColor(const CRTVector&, const CRTTriangle&, float, const CRTVector&)> visualization) const 
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTColor>(imageWidth, scene->getSettings().bgColor));
    std::vector<CRTTriangle> triangles = convertMeshesToTriangles();
    float maxDistanceToCamera = getMaxDistanceToCamera(triangles); // so we know it when visualizing 

    for (int rowId = 0; rowId < imageHeight; rowId++) {
        for (int colId = 0; colId < imageWidth; colId++) {
            CRTRay ray = scene->getCamera().getRayForPixel(rowId, colId);
            CRTVector closestHit;
            CRTTriangle* triangleHit = nullptr;
            float minDistanceToCamera = FLT_MAX;
            bool intersects = false;
            for (int i = 0; i < triangles.size(); i++) {
                std::pair<bool, CRTVector> hit = triangles[i].intersectsRay(ray);
                if (hit.first) {
                    intersects = true;
                    double distanceToCamera = (hit.second - scene->getCamera().getPosition()).length();
                    if (distanceToCamera < minDistanceToCamera) {
                        minDistanceToCamera = distanceToCamera;
                        closestHit = hit.second;
                        triangleHit = &triangles[i];
                    }
                }
            }
            if (intersects && triangleHit) {
                image[rowId][colId] = visualization(
                    closestHit, *triangleHit, maxDistanceToCamera, scene->getCamera().getPosition());
            }
        }
    }
    return image;
}



CRTColor visualizeByDepth(const CRTVector& point, const CRTTriangle& triangle, float maxDistance, const CRTVector& cameraPosition) {
    short colorValue = (1 - ((point - cameraPosition).length() / (maxDistance * 2.2))) * MAX_COLOR_COMPONENT;
    return { colorValue, colorValue, colorValue };
}

CRTColor visualizeByTriangle(const CRTVector& point, const CRTTriangle& triangle, float maxDistance, const CRTVector& cameraPosition) {
    float colorPercentage = (point - cameraPosition).length() / (maxDistance * 2.5);
    CRTColor result = triangle.getColor();
    result.r *= colorPercentage;
    result.g *= colorPercentage;
    result.b *= colorPercentage;

    return result;
}