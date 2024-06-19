#include "CRTRenderer.h"
#include <algorithm>
#include <iostream>


CRTDistanceScene CRTRenderer::sortScene(const CRTScene& scene) const
{
    CRTDistanceScene triangleDistancePair;
    triangleDistancePair.reserve(scene.size());
    for (const CRTTriangle& t : scene) {
        float distance = t.distanceToPoint(screen.getCamera().getPosition());
        triangleDistancePair.emplace_back(abs(distance), &t);
    }
    std::sort(triangleDistancePair.begin(), triangleDistancePair.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.first < rhs.first;
        });
    return triangleDistancePair;
}

CRTRenderer::CRTRenderer(const CRTScreen& screen) : screen(screen)
{}

CRTImage CRTRenderer::renderScene(const CRTScene& scene, 
    std::function<CRTColor(const CRTVector&, const CRTTriangle&, float, const CRTVector&)> visualization) const 
{
    const unsigned imageHeight = screen.getHeight();
    const unsigned imageWidth = screen.getWidth();

    CRTImage image(imageHeight, std::vector<CRTColor>(imageWidth));

    CRTRayMatrix rays = screen.generateRays();
    CRTDistanceScene sortedScene = sortScene(scene);
    // all the triangles are currently sorted by distance descending
    float maxDistance = sortedScene[sortedScene.size() -1 ].first;
    for (int rowId = 0; rowId < imageHeight; rowId++) {
        for (int colId = 0; colId < imageWidth; colId++) {
            for (int i = 0; i < sortedScene.size(); i++) {
                const CRTTriangle& triangle = *sortedScene[i].second;
                std::pair<bool, CRTVector> hit = rays[rowId][colId].intersectsTriangle(triangle);
                if (hit.first) {
                    image[rowId][colId] = visualization(hit.second, triangle, maxDistance, 
                        screen.getCamera().getPosition());
                    break;
                }
            }
            
        }
    }
    return image;
}



CRTColor visualizeByDepth(const CRTVector& point, const CRTTriangle& triangle, float maxDistance, const CRTVector& cameraPosition) {
    short colorValue = (1 - (point.length() / (maxDistance * 2))) * MAX_COLOR_COMPONENT;
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