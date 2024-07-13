#include "CRTRenderer.h"
#include <algorithm>
#include <iostream>
#include <climits>
#include <assert.h>
#include <thread>
#include "Scene/CRTBucketQueue.h"

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

void CRTRenderer::renderRegionNoAABB(int x, int y, int width, int height, CRTImage& output) const
{
    for (int rowId = y; rowId < y + height; rowId++) {
        for (int colId = x; colId < x + width; colId++) {
            CRTRay ray = scene->getCamera().getRayForPixel(rowId, colId); // we generate the ray "on demand"
            output[rowId][colId] = shade(ray, rayTrace(ray));
        }
    }
}

void CRTRenderer::renderRegion(int x, int y, int width, int height, CRTImage& output) const
{
    for (int rowId = y; rowId < y + height; rowId++) {
        for (int colId = x; colId < x + width; colId++) {
            CRTRay ray = scene->getCamera().getRayForPixel(rowId, colId); 
            if (scene->getAABB().intersects(ray)) {
                output[rowId][colId] = shade(ray, rayTrace(ray));
            }
        }
    }
}

void CRTRenderer::renderRegionAccelerated(int x, int y, int width, int height, CRTImage& output) const
{
    for (int rowId = y; rowId < y + height; rowId++) {
        for (int colId = x; colId < x + width; colId++) {
            CRTRay ray = scene->getCamera().getRayForPixel(rowId, colId);
            output[rowId][colId] = shade(ray, rayTraceAccelerated(ray));
        }
    }
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

Intersection CRTRenderer::rayTraceAccelerated(const CRTRay& ray, float maxDistance) const
{
    return scene->getAccelerationStructure().intersect(ray, maxDistance);
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
            finalColor += scene->getGeometryObject(data.hitObjectIndex).sampleMaterial(scene->getMaterial(data.materialIndex), data) * multValue;
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

    return shade(reflectedRay, rayTrace(reflectedRay)) * scene->getGeometryObject(data.hitObjectIndex).sampleMaterial(scene->getMaterial(data.materialIndex), data);
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
    float R0 = pow((n1 - n2) / (n1 + n2), 2);
    float fresnel = R0 + (1 - R0) * pow(1.0f - cosIncomming, 5);

    return fresnel * shade(reflectedRay, rayTrace(reflectedRay)) + (1 - fresnel) * shade(refractedRay, rayTrace(refractedRay));
}


bool CRTRenderer::intersectsObject(const CRTRay& ray, float distanceToLight) const
{
    return rayTraceAccelerated(ray, distanceToLight).hitObjectIndex != NO_HIT_INDEX;
    /*TypeIntersectionMap intersectionsPerType = scene->getAccelerationStructure().intersectPerType(ray, distanceToLight);
    if (intersectionsPerType.count(CRTMaterialType::DIFFUSE) > 0 || intersectionsPerType.count(CRTMaterialType::REFLECTIVE)) {
        return true;
    }
    return false;*/
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

CRTImage CRTRenderer::renderSinglethreaded() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));
    renderRegionNoAABB(0, 0, imageWidth, imageHeight, image);
    return image;
}

CRTImage CRTRenderer::renderByRegions() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));
    int threadCount = std::thread::hardware_concurrency();
    unsigned threadsPerSideY = sqrt(threadCount);
    unsigned threadsPerSideX = threadCount / threadsPerSideY;
    unsigned heightPerThread = imageHeight / threadsPerSideY;
    unsigned widthPerThread = imageWidth / threadsPerSideX; // written like so to avoid the issue of having a non-perfect square number of threads

    std::vector<std::thread> threads;
    for (int i = 0; i < threadCount; i++) {
        unsigned startX = (i * widthPerThread) % imageWidth;
        unsigned startY = (i / threadsPerSideX) * heightPerThread;
        threads.push_back(std::thread(&CRTRenderer::renderRegionNoAABB, this,
           startX, startY, widthPerThread, heightPerThread, std::ref(image))
        );
    }
    for (auto& thread : threads) {
        thread.join();
    }
    return image;
}

CRTImage CRTRenderer::renderByBuckets() const
{
    if (scene->getSettings().imageSettings.bucketSize <= 0) { // the size is invalid
        return renderByRegions();
    }

    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));

    CRTBucketQueue queue(scene); // the constructor generates the regions

    int threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (int i = 0; i < threadCount; i++) {
        threads.push_back(std::thread([this, &queue, &image]() {
            CRTRegion region;
            while (true) {
                if (!queue.getRegion(region)) { // if getRegion returns false, then there's no more buckets
                    break;
                }
                renderRegionNoAABB(region.startX, region.startY, region.width, region.height, image);
                }
            }
        ));
    }

    for (auto& thread : threads) {
        thread.join();
    }
    return image;
}

CRTImage CRTRenderer::renderWithAABB() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));

    CRTBucketQueue queue(scene); // the constructor generates the regions

    int threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (int i = 0; i < threadCount; i++) {
        threads.push_back(std::thread([this, &queue, &image]() {
            CRTRegion region;
            while (true) {
                if (!queue.getRegion(region)) { // if getRegion returns false, then there's no more buckets
                    break;
                }
                renderRegion(region.startX, region.startY, region.width, region.height, image);
            }
            }
        ));
    }

    for (auto& thread : threads) {
        thread.join();
    }
    return image;
}

CRTImage CRTRenderer::renderAccelerated() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));

    CRTBucketQueue queue(scene); // the constructor generates the regions

    int threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (int i = 0; i < threadCount; i++) {
        threads.push_back(std::thread([this, &queue, &image]() {
            CRTRegion region;
            while (true) {
                if (!queue.getRegion(region)) { // if getRegion returns false, then there's no more buckets
                    break;
                }
                renderRegionAccelerated(region.startX, region.startY, region.width, region.height, image);
            }
            }
        ));
    }

    for (auto& thread : threads) {
        thread.join();
    }
    return image;
}

CRTImage CRTRenderer::renderAcceleratedSinglethreaded() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));
    renderRegionAccelerated(0, 0, imageWidth, imageHeight, image);
    return image;
}
