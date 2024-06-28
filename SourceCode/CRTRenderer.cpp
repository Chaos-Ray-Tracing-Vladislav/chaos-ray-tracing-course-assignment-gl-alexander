#include "CRTRenderer.h"
#include <algorithm>
#include <iostream>
#include <climits>

CRTRenderer::CRTRenderer(const CRTScene* scene) : scene(scene)
{}


void CRTRenderer::renderScene(const char* outputname) const
{
    CRTImage img = renderScene();
    CRTImageSaver::saveImage(outputname, img);
}

CRTImage CRTRenderer::renderScene() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTColor>(imageWidth, scene->getSettings().bgColor));

    for (int rowId = 0; rowId < imageHeight; rowId++) {
        for (int colId = 0; colId < imageWidth; colId++) {
            CRTRay ray = scene->getCamera().getRayForPixel(rowId, colId); // we generate the ray "on demand"
            CRTVector closestHit;
            CRTTriangle triangleHit;
            float minDistanceToCamera = FLT_MAX;
            bool intersects = false;
            for (int i = 0; i < scene->getGeometryObjects().size(); i++) { 
                std::tuple<bool, CRTVector, CRTTriangle> hit = scene->getGeometryObjects()[i].intersectsRay(ray);
                if (std::get<bool>(hit)) {
                    intersects = true;
                    double distanceToCamera = (std::get<CRTVector>(hit) - scene->getCamera().getPosition()).length();
                    if (distanceToCamera < minDistanceToCamera) {
                        minDistanceToCamera = distanceToCamera;
                        closestHit = std::get<CRTVector>(hit);
                        triangleHit = std::get<CRTTriangle>(hit);
                    }
                }
            }
            if (intersects) {
                image[rowId][colId] = shade(closestHit, triangleHit); 
            }
            else {
                image[rowId][colId] = scene->getSettings().bgColor;
            }
        }
    }
    return image;
}

CRTColor CRTRenderer::shade(const CRTVector& point, const CRTTriangle& triangleHit) const
{
    CRTColor finalColor = {0, 0, 0};
    for (const CRTLight& light : scene->getLights()) {
        CRTVector lightDirection = light.getPosition() - point;
        float sphereRadius = lightDirection.length();
        float sphereArea = 4 * PI * sphereRadius * sphereRadius;
        lightDirection.normalize();
        float cosLaw = std::max(0.0f, dot(lightDirection, triangleHit.getNormal()));
        CRTRay shadowRay(point + triangleHit.getNormal() * SHADOW_BIAS, lightDirection);
        if (!intersectsObject(shadowRay)) {
            float multValue = light.getIntensity() / sphereArea * cosLaw;
            finalColor += ALBEDO * multValue;
        }
    }
    return finalColor;
}

bool CRTRenderer::intersectsObject(const CRTRay& ray) const
{
    for (int i = 0; i < scene->getGeometryObjects().size(); i++) {
        if (scene->getGeometryObjects()[i].intersectsShadowRay(ray)) {
            return true;
        }
    }
    return false;
}

CRTColor visualizeByDepth(const CRTVector& point, const CRTVector& cameraPosition) {
    short colorValue = (1 - ((point - cameraPosition).length() / MAX_RENDER_DISTANCE)) * MAX_COLOR_COMPONENT;
    return { colorValue, colorValue, colorValue };
}

