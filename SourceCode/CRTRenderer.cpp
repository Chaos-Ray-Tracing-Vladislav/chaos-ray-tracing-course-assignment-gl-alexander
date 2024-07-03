#include "CRTRenderer.h"
#include <algorithm>
#include <iostream>
#include <climits>
#include <assert.h>

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

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));

    for (int rowId = 0; rowId < imageHeight; rowId++) {
        for (int colId = 0; colId < imageWidth; colId++) {
            CRTRay ray = scene->getCamera().getRayForPixel(rowId, colId); // we generate the ray "on demand"
            image[rowId][colId] = shade(ray, rayTrace(ray));
        }
    }
    return image;
}

Intersection CRTRenderer::rayTrace(const CRTRay& ray) const
{
    Intersection intersection;
    Intersection curr_intersection;
    float minDistanceToCamera = FLT_MAX;
    for (int i = 0; i < scene->getObjectsCount(); i++) {
        curr_intersection = scene->getGeometryObject(i).intersectsRay(ray);
        if (curr_intersection.triangleIndex != NO_HIT_INDEX) {
            double distanceToCamera = (curr_intersection.hitPoint - ray.origin).length();
            if (distanceToCamera < minDistanceToCamera) {
                minDistanceToCamera = distanceToCamera;
                intersection = std::move(curr_intersection);
                intersection.hitObjectIndex = i; // here we set the mesh it hits
                intersection.materialIndex = scene->getGeometryObject(i).getMaterialIndex();
            }
        }
    }
    return intersection;
}

CRTVector CRTRenderer::shade(const CRTRay& ray, const Intersection& data) const
{
    if (data.hitObjectIndex == NO_HIT_INDEX || ray.depth >= MAX_RAY_DEPTH) {
        return scene->getSettings().bgColor;
    }
    const CRTMaterial& material = scene->getMaterial(data.materialIndex);
    if (material.type == CRTMaterialType::DIFFUSE) {
        return shadeDiffuse(ray, data);
    }
    else if (material.type == CRTMaterialType::REFLECTIVE) {
        return shadeReflective(ray, data);
    }
    else {
        assert(false);
    }
}

CRTVector CRTRenderer::shadeDiffuse(const CRTRay& ray, const Intersection& data) const {
    CRTVector finalColor = { 0, 0, 0 };
    CRTVector normalVector = data.faceNormal;
    const CRTMaterial& material = scene->getMaterial(data.materialIndex);
    if (material.smoothShading) {
        normalVector = data.smoothNormal;
    }
    for (const CRTLight& light : scene->getLights()) {
        CRTVector lightDirection = light.getPosition() - data.hitPoint;
        float sphereRadius = lightDirection.length();
        float sphereArea = 4 * PI * sphereRadius * sphereRadius;
        lightDirection.normalize();
        float cosLaw = std::max(0.0f, dot(lightDirection, normalVector));
        CRTRay shadowRay{ data.hitPoint + normalVector * SHADOW_BIAS, lightDirection, RayType::SHADOW, ray.depth + 1 };
        if (!intersectsObject(shadowRay)) {
            float multValue = light.getIntensity() / sphereArea * cosLaw;
            finalColor += material.albedo * multValue;
        }
    }
    return finalColor;
}

CRTVector CRTRenderer::shadeReflective(const CRTRay& ray, const Intersection& data) const {
    CRTVector normalVector = data.faceNormal;
    const CRTMaterial& material = scene->getMaterial(data.materialIndex);
    if (material.smoothShading) {
        normalVector = data.smoothNormal;
    }
    CRTVector reflectedDirection = reflect(ray.direction, normalVector);
    CRTRay reflectedRay{ data.hitPoint + normalVector * REFLECTION_BIAS, reflectedDirection, RayType::REFLECTIVE, ray.depth + 1 };
    return shade(reflectedRay, rayTrace(reflectedRay)) * material.albedo;
}

bool CRTRenderer::intersectsObject(const CRTRay& ray) const
{
    for (int i = 0; i < scene->getObjectsCount(); i++) {
        if (scene->getGeometryObject(i).intersectsRay(ray).triangleIndex != NO_HIT_INDEX) {
            return true;
        }
    }
    return false;
}


CRTImage CRTRenderer::renderSceneBarycentic() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));

    Intersection intersection;
    for (int rowId = 0; rowId < imageHeight; rowId++) {
        for (int colId = 0; colId < imageWidth; colId++) {
            intersection.triangleIndex = NO_HIT_INDEX;
            CRTRay ray = scene->getCamera().getRayForPixel(rowId, colId); // we generate the ray "on demand"
            float minDistanceToCamera = FLT_MAX;
            for (int i = 0; i < scene->getObjectsCount(); i++) {
                intersection = scene->getGeometryObject(i).intersectsRay(ray);
                if (intersection.triangleIndex != NO_HIT_INDEX) {
                    double distanceToCamera = (intersection.hitPoint - scene->getCamera().getPosition()).length();
                    if (distanceToCamera < minDistanceToCamera) {
                        minDistanceToCamera = distanceToCamera;
                    }
                }
            }
            if (intersection.triangleIndex != NO_HIT_INDEX) {
                image[rowId][colId] = intersection.barycentricCoordinates;
            }
            else {
                image[rowId][colId] = scene->getSettings().bgColor;
            }
        }
    }
    return image;
}