#include "CRTRenderer.h"
#include <algorithm>
#include <iostream>


CRTRenderer::CRTRenderer(const CRTScreen& screen, const CRTVector& cameraPosition) : screen(screen), cameraPosition(cameraPosition) 
{}

CRTImage CRTRenderer::renderScene(const CRTScene& scene, 
    std::function<CRTColor(const CRTVector&, const CRTTriangle&, float)> visualization) const 
{
    const unsigned imageHeight = screen.getHeight();
    const unsigned imageWidth = screen.getWidth();

    CRTImage image(imageHeight, std::vector<CRTColor>(imageWidth));

    CRTRayMatrix rays = screen.generateRays(cameraPosition);

    std::vector<std::pair<float, const CRTTriangle*>> triangleDistancePair;
    triangleDistancePair.reserve(scene.size());
    for (const CRTTriangle& t : scene) {
        float distance = t.distanceToPoint(cameraPosition);
        triangleDistancePair.emplace_back(distance, &t);
    }
    std::sort(triangleDistancePair.begin(), triangleDistancePair.end(), 
        [](const auto& lhs, const auto& rhs) {
        return lhs.first > rhs.first;
        });
    // all the triangles are currently sorted by distance descending
    float maxDistance = triangleDistancePair[0].first;
    for (int rowId = 0; rowId < imageHeight; rowId++) {
        for (int colId = 0; colId < imageWidth; colId++) {
            for (int i = 0; i < triangleDistancePair.size(); i++) {
                const CRTTriangle& triangle = *triangleDistancePair[i].second;
                std::pair<bool, CRTVector> hit = rays[rowId][colId].intersectsTriangle(triangle);
                if (hit.first) {
                    image[rowId][colId] = visualization(hit.second, triangle, maxDistance);
                }
            }
            
        }
    }
    return image;
}
