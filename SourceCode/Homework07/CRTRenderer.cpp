#include "CRTRenderer.h"
#include <algorithm>
#include <iostream>


CRTDistanceScene CRTRenderer::sortScene(const std::vector<CRTTriangle>& triangles) const
{
    CRTDistanceScene triangleDistancePair;
    triangleDistancePair.reserve(triangles.size());
    for (const CRTTriangle& t : triangles) {
        float distance = t.distanceToPoint(scene->getCamera().getPosition());
        triangleDistancePair.emplace_back(abs(distance), &t);
    }
    std::sort(triangleDistancePair.begin(), triangleDistancePair.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.first < rhs.first;
        });
    return triangleDistancePair;
}

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


CRTImage CRTRenderer::renderScene(std::function<CRTColor(const CRTVector&, const CRTTriangle&, float, const CRTVector&)> visualization) const 
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTColor>(imageWidth, scene->getSettings().bgColor));

    CRTRayMatrix rays = scene->getCamera().generateRays();
    std::vector<CRTTriangle> triangles = convertMeshesToTriangles();
    CRTDistanceScene sortedTriangles = sortScene(triangles);
    // all the triangles are currently sorted by distance descending
    float maxDistance = sortedTriangles[sortedTriangles.size() -1 ].first;
    for (int rowId = 0; rowId < imageHeight; rowId++) {
        for (int colId = 0; colId < imageWidth; colId++) {
            for (int i = 0; i < sortedTriangles.size(); i++) {
                const CRTTriangle& triangle = *sortedTriangles[i].second;
                std::pair<bool, CRTVector> hit = rays[rowId][colId].intersectsTriangle(triangle);
                if (hit.first) {
                    image[rowId][colId] = visualization(
                        hit.second, triangle, maxDistance, scene->getCamera().getPosition());
                    break;
                }
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