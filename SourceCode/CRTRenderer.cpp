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

void CRTRenderer::renderSceneBarycentic(const char* outputname) const
{
    CRTImage img = renderSceneBarycentic();
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
            image[rowId][colId] = rayTrace(ray, 0, CRTVector(1, 1, 1)); // start from depth 0 and standard albedo values
        }
    }
    return image;
}

CRTColor CRTRenderer::rayTrace(const CRTRay& ray, int depth, const CRTVector& addedAlbedo) const
{
    CRTVector closestHit;
    CRTVector normalVector;
    const CRTMesh* meshHit = nullptr;
    float minDistanceToCamera = FLT_MAX;
    bool intersects = false;
    for (int i = 0; i < scene->getGeometryObjects().size(); i++) {
        std::tuple<bool, CRTVector, CRTVector> hit = scene->getGeometryObjects()[i].intersectsRay(ray);
        if (std::get<0>(hit)) {
            intersects = true;
            double distanceToCamera = (std::get<1>(hit) - ray.getOrigin()).length();
            if (distanceToCamera < minDistanceToCamera) {
                minDistanceToCamera = distanceToCamera;
                closestHit = std::get<1>(hit);
                normalVector = std::get<2>(hit);
                meshHit = &scene->getGeometryObjects()[i];
            }
        }
    }
    if (intersects && meshHit) {
        const CRTVector& reflectiveAlbedo = meshHit->getMaterial().albedo;
        CRTVector newAlbedo(addedAlbedo.x * reflectiveAlbedo.x, addedAlbedo.y * reflectiveAlbedo.y, addedAlbedo.z * reflectiveAlbedo.z);
        if (meshHit->getMaterial().type == CRTMaterialType::DIFFUSE || depth >= REFLECTION_DEPTH) {
            return shade(closestHit, normalVector, newAlbedo);
        }
        else if (meshHit->getMaterial().type == CRTMaterialType::REFLECTIVE) {
            CRTVector reflectedDirection = reflect(ray.getDirection(), normalVector);
            CRTRay lightRay(closestHit, reflectedDirection);
            return rayTrace(lightRay, depth + 1, newAlbedo);
        }
    }
    else {
        return {
            (short)(addedAlbedo.x * scene->getSettings().bgColor.r),
            (short)(addedAlbedo.y * scene->getSettings().bgColor.g),
            (short)(addedAlbedo.z * scene->getSettings().bgColor.b) };
    }
}

CRTColor CRTRenderer::shade(const CRTVector& point, const CRTVector& normalVector, const CRTVector& albedo) const
{
    CRTColor finalColor = {0, 0, 0};
    for (const CRTLight& light : scene->getLights()) {
        CRTVector lightDirection = light.getPosition() - point;
        float sphereRadius = lightDirection.length();
        float sphereArea = 4 * PI * sphereRadius * sphereRadius;
        lightDirection.normalize();
        float cosLaw = std::max(0.0f, dot(lightDirection, normalVector));
        CRTRay shadowRay(point + normalVector * SHADOW_BIAS, lightDirection);
        if (!intersectsObject(shadowRay)) {
            float multValue = light.getIntensity() / sphereArea * cosLaw ;
            CRTVector colorContribution = albedo * multValue * MAX_COLOR_COMPONENT;
            finalColor += {(short)colorContribution.x, (short)colorContribution.y, (short)colorContribution.z};
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


CRTImage CRTRenderer::renderSceneBarycentic() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTColor>(imageWidth, scene->getSettings().bgColor));

    for (int rowId = 0; rowId < imageHeight; rowId++) {
        for (int colId = 0; colId < imageWidth; colId++) {
            CRTRay ray = scene->getCamera().getRayForPixel(rowId, colId); // we generate the ray "on demand"
            CRTVector barycenticCoords;
            float minDistanceToCamera = FLT_MAX;
            bool intersects = false;
            for (int i = 0; i < scene->getGeometryObjects().size(); i++) {
                std::tuple<bool, CRTVector, CRTVector, CRTVector> hit = scene->getGeometryObjects()[i].intersectsRayBarycentic(ray);
                if (std::get<bool>(hit)) {
                    intersects = true;
                    double distanceToCamera = (std::get<1>(hit) - scene->getCamera().getPosition()).length();
                    if (distanceToCamera < minDistanceToCamera) {
                        minDistanceToCamera = distanceToCamera;
                        barycenticCoords = std::get<3>(hit);
                    }
                }
            }
            if (intersects) {
                CRTVector colorContribution = barycenticCoords * MAX_COLOR_COMPONENT;
                image[rowId][colId] = { (short)colorContribution.x, (short)colorContribution.y, (short)colorContribution.z };
            }
            else {
                image[rowId][colId] = scene->getSettings().bgColor;
            }
        }
    }
    return image;
}


CRTColor visualizeByDepth(const CRTVector& point, const CRTVector& cameraPosition) {
    short colorValue = (1 - ((point - cameraPosition).length() / MAX_RENDER_DISTANCE)) * MAX_COLOR_COMPONENT;
    return { colorValue, colorValue, colorValue };
}

