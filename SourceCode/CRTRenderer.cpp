#include "CRTRenderer.h"
#include <algorithm>
#include <iostream>
#include <climits>
#include <assert.h>

static float clamp(float original, float start, float end) {
    if (original < start) return start;
    if (original > end) return end;
    return original;
}

CRTRenderer::CRTRenderer(const CRTScene* scene) : scene(scene)
{}

void CRTRenderer::renderScene(const char* outputname) const
{
    CRTImageSaver::saveImage(outputname, renderScene());
}

void CRTRenderer::renderSceneBarycentic(const char* outputname) const
{
    CRTImageSaver::saveImage(outputname, renderSceneBarycentic());
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
    float minDistanceToOrigin = FLT_MAX;
    for (int i = 0; i < scene->getObjectsCount(); i++) {
        curr_intersection = scene->getGeometryObject(i).intersectsRay(ray);
        if (curr_intersection.triangleIndex != NO_HIT_INDEX) {
            double distanceToOrigin = (curr_intersection.hitPoint - ray.origin).length();
            if (distanceToOrigin < minDistanceToOrigin) {
                minDistanceToOrigin = distanceToOrigin;
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
    else if (material.type == CRTMaterialType::REFRACTIVE) {
        return shadeRefractive(ray, data);
    }
    else {
        assert(false);
    }
}

CRTVector CRTRenderer::shadeDiffuse(const CRTRay& ray, const Intersection& data) const {
    CRTVector finalColor{ 0.0, 0.0, 0.0 };
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
        if (!intersectsObject(shadowRay, sphereRadius)) {
            float multValue = light.getIntensity() / sphereArea * cosLaw;
            finalColor += scene->sampleMaterial(data) * multValue;
        }
    }
    return CRTVector(clamp(finalColor.x, 0, 1), 
        clamp(finalColor.y, 0, 1), 
        clamp(finalColor.z, 0, 1));
}

CRTVector CRTRenderer::shadeReflective(const CRTRay& ray, const Intersection& data) const {
    CRTVector normalVector = data.faceNormal;
    const CRTMaterial& material = scene->getMaterial(data.materialIndex);
    if (material.smoothShading) {
        normalVector = data.smoothNormal;
    }
    CRTVector reflectedDirection = reflect(ray.direction, normalVector);
    CRTRay reflectedRay{ data.hitPoint + normalVector * REFLECTION_BIAS, reflectedDirection, RayType::REFLECTIVE, ray.depth + 1 };
    CRTVector uv = scene->getGeometryObject(data.hitObjectIndex).getUV(data);

    return shade(reflectedRay, rayTrace(reflectedRay)) * scene->sampleMaterial(data);
}

CRTVector CRTRenderer::shadeRefractive(const CRTRay& ray, const Intersection& data) const {
    const CRTMaterial& material = scene->getMaterial(data.materialIndex);
    float n1 = 1.0f; // Index of refraction of the air
    float n2 = material.ior;
    CRTVector normalVector = material.smoothShading ? data.smoothNormal : data.faceNormal;

    float dotPr = dot(ray.direction, normalVector);
    if (dotPr > 0) { // Ray is leaving the refractive object
        normalVector *= -1;
        dotPr *= -1;
        std::swap(n1, n2);
    }

    float cosIncomming = -dotPr;
    float sinIncomming = 1.0f - cosIncomming * cosIncomming;

    if (sinIncomming > ((n2 * n2) / (n1 * n1))) { // total internal reflection
        CRTVector reflectedDirection = reflect(ray.direction, normalVector);
        CRTRay reflectedRay{ data.hitPoint + normalVector * REFLECTION_BIAS, reflectedDirection, RayType::REFLECTIVE, ray.depth + 1 };
        return shade(reflectedRay, rayTrace(reflectedRay));
    }

    float sinOutcomming = (n1 / n2) * sqrt(std::max(0.0f, 1.0f - cosIncomming * cosIncomming));
    float cosOutcomming = sqrt(std::max(0.0f, 1.0f - sinOutcomming * sinOutcomming));

    CRTVector A = cosOutcomming * (-normalVector);
    CRTVector C = ray.direction + (cosIncomming * normalVector);
    C.normalize();
    CRTVector refractedDirection = A + (C * sinOutcomming);
    refractedDirection.normalize();

    CRTRay refractedRay{ data.hitPoint + normalVector * -REFRACTION_BIAS, refractedDirection, RayType::REFRACTIVE, ray.depth + 1 };

    CRTVector reflectedDirection = reflect(ray.direction, normalVector);
    CRTRay reflectedRay{ data.hitPoint + normalVector * REFLECTION_BIAS, reflectedDirection, RayType::REFLECTIVE, ray.depth + 1 };

    // Improved Fresnel calculation using Schlick's approximation
    //float R0 = pow((n1 - n2) / (n1 + n2), 2);
    //float fresnel = R0 + (1 - R0) * pow(1.0f - cosIncomming, 5);

    // Simple Fresnel calculation
    float fresnel = 0.5 * pow(1.0f - cosIncomming, 5);

    return fresnel * shade(reflectedRay, rayTrace(reflectedRay)) + (1 - fresnel) * shade(refractedRay, rayTrace(refractedRay));
}


bool CRTRenderer::intersectsObject(const CRTRay& ray, float distanceToLight) const
{
    Intersection intersection;
    for (int i = 0; i < scene->getObjectsCount(); i++) {
        intersection = scene->getGeometryObject(i).intersectsRay(ray);
        if (intersection.triangleIndex != NO_HIT_INDEX) {
            if ((ray.origin - intersection.hitPoint).length() < distanceToLight 
                && scene->getMaterial(intersection.materialIndex).type != CRTMaterialType::REFRACTIVE) {
                return true;
            }
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
            intersection = rayTrace(scene->getCamera().getRayForPixel(rowId, colId));
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