#include "CRTRaytracer.h"
#include <algorithm>
#include <iostream>
#include <climits>
#include <assert.h>
#include <thread>

CRTRaytracer::CRTRaytracer(CRTScene* scene) : scene(scene)
{}

void CRTRaytracer::renderScene(const char* outputname) const
{
    CRTImageSaver::saveImage(outputname, renderAccelerated());
}

void CRTRaytracer::renderSceneBarycentic(const char* outputname) const
{
    CRTImageSaver::saveImage(outputname, renderSceneBarycentic());
}

CRTImage CRTRaytracer::renderAccelerated() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));

    CRTBucketArray buckets(scene); // the constructor generates the regions

    int threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (int i = 0; i < threadCount; i++) {
        threads.push_back(std::thread([this, &buckets, &image]() {
            CRTRegion region;
            while (buckets.nextRegion(region)) {
                if (scene->getSettings().renderedAA) {
                    renderRegionPixelGrid(region.startX, region.startY, region.width, region.height, image);
                    //renderRegionMonteCarloAA(region.startX, region.startY, region.width, region.height, image);
                    // the Monte Carlo AA chooses spots on the pixel at random, while the Pixel Grid uses a more uniform distribution
                }
                else {
                    if (scene->getSettings().stereoscopy) {
                        if (scene->getSettings().dof) {
                            renderRegionDOFStereoscopy(region.startX, region.startY, region.width, region.height, image);
                        }
                        else {
                            renderRegionStereoscopy(region.startX, region.startY, region.width, region.height, image);
                        }
                    }
                    else {
                        if (scene->getSettings().dof) {
                            renderRegionFocalBlur(region.startX, region.startY, region.width, region.height, image);
                        }
                        else {
                            renderRegion(region.startX, region.startY, region.width, region.height, image);
                        }
                    }
                }
            }
            }));
    }

    for (auto& thread : threads) {
        thread.join();
    }
    if (scene->getSettings().FXAA) {
        FXAA::applyFXAA(image);
    }
    return image;
}

CRTImage CRTRaytracer::renderAcceleratedSinglethreaded() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));
    if (scene->getSettings().renderedAA) {
        renderRegionPixelGrid(0, 0, imageWidth, imageHeight, image);
        //renderRegionMonteCarlo(0, 0, imageWidth, imageHeight, image);
    }
    else {
        if (scene->getSettings().stereoscopy) {
            if (scene->getSettings().dof) {
                renderRegionDOFStereoscopy(0, 0, imageWidth, imageHeight, image);
            }
            else {
                renderRegionStereoscopy(0, 0, imageWidth, imageHeight, image);
            }
        }
        else {
            if (scene->getSettings().dof) {
                renderRegionFocalBlur(0, 0, imageWidth, imageHeight, image);
            }
            else {
                renderRegion(0, 0, imageWidth, imageHeight, image);
            }
        }
    }
    if (scene->getSettings().FXAA) {
        FXAA::applyFXAA(image);
    }
    return image;
}

void CRTRaytracer::renderAnimation(const char* outputname, std::vector<CRTCamera> keyframes) const
{
    for (int i = 0; i < keyframes.size(); i++) {
        char framename[128];
        sprintf_s(framename, "%s_frame_%03d.ppm", outputname, i);
        scene->setCamera(keyframes[i]);
        renderScene(framename);
        std::cout << "Rendered " << framename << std::endl;
    }
}

void CRTRaytracer::renderRegionNoAABB(int x, int y, int width, int height, CRTImage& output) const
{
    for (int rowId = y; rowId < y + height; rowId++) {
        for (int colId = x; colId < x + width; colId++) {
            CRTRay ray = scene->getCamera().getRayForPixel(rowId, colId);
            output[rowId][colId] = shade(ray, rayTrace(ray));
        }
    }
}

void CRTRaytracer::renderRegionSimple(int x, int y, int width, int height, CRTImage& output) const
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

void CRTRaytracer::renderRegion(int x, int y, int width, int height, CRTImage& output) const
{
    for (int rowId = y; rowId < y + height; rowId++) {
        for (int colId = x; colId < x + width; colId++) {
            CRTRay ray = scene->getCamera().getRayForPixel(rowId, colId);
            output[rowId][colId] = shade(ray, rayTraceAccelerated(ray));
        }
    }
}

void CRTRaytracer::renderRegionMonteCarloAA(int x, int y, int width, int height, CRTImage& output) const
{
    float mult = 1.0f / scene->getSettings().raysPerPixel;
    for (int rowId = y; rowId < y + height; rowId++) {
        for (int colId = x; colId < x + width; colId++) {
            CRTVector finalColor(0, 0, 0);
            for (int i = 0; i < scene->getSettings().raysPerPixel; i++) {
                CRTRay ray = scene->getCamera().getRayForSubpixel(rowId + randFloat(), colId + randFloat());
                finalColor += shade(ray, rayTraceAccelerated(ray));
            }
            output[rowId][colId] = finalColor * mult;
        }
    }
}

void CRTRaytracer::renderRegionPixelGrid(int x, int y, int width, int height, CRTImage& output) const
{
    float mult = 1.0f / scene->getSettings().raysPerPixel;
    int squareCount = scene->getSettings().raysPerPixel - 1;
    int squaresPerSide = sqrt(squareCount);
    float squareCenterX, squareCenterY, side, halfside;
    side = 1.0f / squaresPerSide;
    halfside = side / 2.0f;
    for (int rowId = y; rowId < y + height; rowId++) {
        for (int colId = x; colId < x + width; colId++) {
            CRTVector finalColor(0, 0, 0);
            for (int x = 0; x < squaresPerSide; x++) {
                for (int y = 0; y < squaresPerSide; y++) {
                    squareCenterX = x * side + halfside;
                    squareCenterY = y * side + halfside;
                    CRTRay ray = scene->getCamera().getRayForSubpixel(rowId + squareCenterY, colId + squareCenterX);
                    finalColor += shade(ray, rayTraceAccelerated(ray));
                }
            }
            for (int i = squareCount; i < scene->getSettings().raysPerPixel; i++) {
                // generate the rest using montecarlo
                CRTRay ray = scene->getCamera().getRayForSubpixel(rowId + randFloat(), colId + randFloat());
                finalColor += shade(ray, rayTraceAccelerated(ray));
            }
            output[rowId][colId] = finalColor * mult;
        }
    }
}

void CRTRaytracer::renderRegionFocalBlur(int x, int y, int width, int height, CRTImage& output) const
{
    const CRTCamera& camera = scene->getCamera();
    float u, v, mult, aperature, focalDistance;
    if (scene->getSettings().autoFocus) {
        // if we autofocus, we'll focus at the center of the scene
        focalDistance = ((scene->getAABB().max - scene->getAABB().min) / 2.0 - camera.getPosition()).length();
    }
    else {
        focalDistance = scene->getSettings().focalPlaneDist;
    }

    mult = 1.0f / scene->getSettings().dofSamples;
    aperature = 1.0f / scene->getSettings().fNum;
    for (int rowId = y; rowId < y + height; rowId++) {
        for (int colId = x; colId < x + width; colId++) {
            CRTRay ray = camera.getRayForPixel(rowId, colId);
            CRTVector finalColor(0, 0, 0);
            for (int i = 0; i < scene->getSettings().dofSamples; i++) {
                float M = focalDistance / dot(camera.getFrontDirection(), ray.direction);
                CRTVector T = camera.getPosition() + ray.direction * M;

                randomCircleSample(u, v);
                u *= aperature;
                v *= aperature;
                ray.origin = scene->getCamera().getPosition() + u * camera.getRightDirection() + v * camera.getUpDirection();
                ray.direction = (T - ray.origin).normalize();
                finalColor += shade(ray, rayTraceAccelerated(ray));
            }
            output[rowId][colId] = finalColor * mult;
        }
    }
}

void CRTRaytracer::renderRegionStereoscopy(int x, int y, int width, int height, CRTImage& output) const
{
    float eyeDistance = scene->getSettings().eyeDistance;
    if (eyeDistance <= EPSILON) return; 
    const CRTVector& leftEyeColor = scene->getSettings().leftEyeColor;
    const CRTVector& rightEyeColor = scene->getSettings().rightEyeColor;
    for (int rowId = y; rowId < y + height; rowId++) {
        for (int colId = x; colId < x + width; colId++) {
            auto rayPair = scene->getCamera().getEyeRays(rowId, colId, eyeDistance);
            CRTVector colorLeft = shade(rayPair.first, rayTraceAccelerated(rayPair.first)) * DESATURIZATION * leftEyeColor;
            CRTVector colorRight = shade(rayPair.second, rayTraceAccelerated(rayPair.second)) * DESATURIZATION * rightEyeColor;
            output[rowId][colId] = colorLeft + colorRight;
        }
    }
}

void CRTRaytracer::renderRegionDOFStereoscopy(int x, int y, int width, int height, CRTImage& output) const
{
    float eyeDistance = scene->getSettings().eyeDistance;
    if (eyeDistance <= EPSILON) return;
    const CRTCamera& camera = scene->getCamera();
    float u, v, mult, aperature, focalDistance;
    if (scene->getSettings().autoFocus) {
        // if we autofocus, we'll focus at the center of the scene
        focalDistance = ((scene->getAABB().max - scene->getAABB().min) / 2.0 - camera.getPosition()).length();
    }
    else {
        focalDistance = scene->getSettings().focalPlaneDist;
    }

    mult = 1.0f / scene->getSettings().dofSamples;
    aperature = 1.0f / scene->getSettings().fNum;
    const CRTVector& leftEyeColor = scene->getSettings().leftEyeColor;
    const CRTVector& rightEyeColor = scene->getSettings().rightEyeColor;
    for (int rowId = y; rowId < y + height; rowId++) {
        for (int colId = x; colId < x + width; colId++) {
            CRTVector finalColor(0, 0, 0);
            for (int i = 0; i < scene->getSettings().dofSamples; i++) {
                auto rayPair = scene->getCamera().getEyeRays(rowId, colId, eyeDistance);

                float M = focalDistance / dot(camera.getFrontDirection(), rayPair.first.direction);
                // both M values are the same, so we reuse them
                CRTVector T_left = rayPair.first.origin + rayPair.first.direction * M;
                CRTVector T_right = rayPair.second.origin + rayPair.second.direction * M;

                randomCircleSample(u, v);
                u *= aperature;
                v *= aperature;
                rayPair.first.origin = rayPair.first.origin + u * camera.getRightDirection() + v * camera.getUpDirection();
                rayPair.second.origin = rayPair.second.origin + u * camera.getRightDirection() + v * camera.getUpDirection();

                rayPair.first.direction = (T_left - rayPair.first.origin).normalize();
                rayPair.second.direction = (T_right - rayPair.second.origin).normalize();

                CRTVector colorLeft = shade(rayPair.first, rayTraceAccelerated(rayPair.first)) * DESATURIZATION * leftEyeColor;
                CRTVector colorRight = shade(rayPair.second, rayTraceAccelerated(rayPair.second)) * DESATURIZATION * rightEyeColor;

                finalColor += colorLeft + colorRight;
            }
            output[rowId][colId] = finalColor * mult;
        }
    }
}


Intersection CRTRaytracer::rayTrace(const CRTRay& ray) const
{
    Intersection intersection;
    Intersection curr_intersection;
    float minDistanceToOrigin = FLOAT_MAX;
    for (int i = 0; i < scene->getObjectsCount(); i++) {
        curr_intersection = scene->getGeometryObject(i).intersectsRay(ray);
        if (curr_intersection.triangleIndex != NO_HIT_INDEX) {
            if (curr_intersection.t < minDistanceToOrigin) {
                minDistanceToOrigin = curr_intersection.t;
                intersection = std::move(curr_intersection);
                intersection.hitObjectIndex = i; // here we set the mesh it hits
                intersection.materialIndex = scene->getGeometryObject(i).getMaterialIndex();
            }
        }
    }
    return intersection;
}

Intersection CRTRaytracer::rayTraceAccelerated(const CRTRay& ray, float maxDistance) const
{
    return scene->getAccelerationStructure().intersect(ray, maxDistance);
}


bool CRTRaytracer::intersectsObject(const CRTRay& ray, float distanceToLight) const
{
    return rayTraceAccelerated(ray, distanceToLight).hitObjectIndex != NO_HIT_INDEX;
}

CRTImage CRTRaytracer::renderSceneBarycentic() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));

    Intersection intersection;
    for (int rowId = 0; rowId < imageHeight; rowId++) {
        for (int colId = 0; colId < imageWidth; colId++) {
            intersection = rayTraceAccelerated(scene->getCamera().getRayForPixel(rowId, colId));
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

CRTImage CRTRaytracer::renderSinglethreaded() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));
    renderRegionNoAABB(0, 0, imageWidth, imageHeight, image);
    return image;
}

CRTImage CRTRaytracer::renderByRegions() const
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
        threads.push_back(std::thread(&CRTRaytracer::renderRegion, this,
           startX, startY, widthPerThread, heightPerThread, std::ref(image))
        );
    }
    for (auto& thread : threads) {
        thread.join();
    }
    return image;
}

CRTImage CRTRaytracer::renderByBuckets() const
{
    if (scene->getSettings().imageSettings.bucketSize <= 0) { // the size is invalid
        return renderByRegions();
    }

    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));
     CRTBucketArray buckets(scene); // the constructor generates the regions
    int bucketsCount = buckets.size();
    std::atomic_int bucketIndex = 0;

    int threadCount = std::thread::hardware_concurrency();
    std::vector<std::jthread> threads;

    for (int i = 0; i < threadCount; i++) {
        //threads.emplace_back(&CRTRaytracer::threadLoop, this, std::ref(image), std::ref(queue));
        threads.push_back(std::jthread([this, &buckets, &bucketsCount, &bucketIndex, &image]() {
            CRTRegion region;
            int idx = 0;
            while ((idx = bucketIndex.fetch_add(1)) < bucketsCount) {
                region = buckets.getRegion(idx);
                renderRegionNoAABB(region.startX, region.startY, region.width, region.height, image);
            }
            }));
    }

    return image;
}

CRTImage CRTRaytracer::renderWithAABB() const
{
    const unsigned imageHeight = scene->getSettings().imageSettings.height;
    const unsigned imageWidth = scene->getSettings().imageSettings.width;

    CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));

    CRTBucketArray buckets(scene); // the constructor generates the regions
    int bucketsCount = buckets.size();
    std::atomic_int bucketIndex = 0;

    int threadCount = std::thread::hardware_concurrency();
    std::vector<std::jthread> threads;

    for (int i = 0; i < threadCount; i++) {
        //threads.emplace_back(&CRTRaytracer::threadLoop, this, std::ref(image), std::ref(queue));
        threads.push_back(std::jthread([this, &buckets, &bucketsCount, &bucketIndex, &image]() {
            CRTRegion region;
            int idx = 0;
            while ((idx = bucketIndex.fetch_add(1)) < bucketsCount) {
                region = buckets.getRegion(idx);
                renderRegion(region.startX, region.startY, region.width, region.height, image);
            }
            }));
    }

    return image;
}

CRTVector CRTRaytracer::shade(const CRTRay& ray, const Intersection& data) const
{
    if (data.hitObjectIndex == NO_HIT_INDEX || ray.depth >= MAX_RAY_DEPTH) {
        return scene->getSettings().bgColor;
    }
    const CRTMaterial& material = scene->getMaterial(data.materialIndex);
    if (material.type == CRTMaterialType::DIFFUSE) {
        return shadeDiffuse(ray, data);
    }
    else if (material.type == CRTMaterialType::REFLECTIVE) {
        if (scene->getSettings().reflections) 
            return shadeReflective(ray, data);
    }
    else if (material.type == CRTMaterialType::REFRACTIVE) {
        if (scene->getSettings().refractions) 
            return shadeRefractive(ray, data);
    }
    else {
        assert(false);
    }
    return scene->getSettings().bgColor;
}

CRTVector CRTRaytracer::shadeDiffuse(const CRTRay& ray, const Intersection& data) const {
    
    if (scene->getSettings().globalIllumination) {
        return (shadeDirectIllumination(ray, data) + shadeGlobalIllumination(ray, data)) / (float)(GI_RAYS + 1);
    }
    return shadeDirectIllumination(ray, data);
}

CRTVector CRTRaytracer::shadeDirectIllumination(const CRTRay& ray, const Intersection& data) const
{
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
    return finalColor.clamp(0, 1);
}

CRTVector CRTRaytracer::shadeGlobalIllumination(const CRTRay& ray, const Intersection& data) const
{
    CRTVector diffuseReflections{ 0.0, 0.0, 0.0 };
    CRTVector normalVector = data.faceNormal;
    const CRTMaterial& material = scene->getMaterial(data.materialIndex);
    if (material.smoothShading) {
        normalVector = data.smoothNormal;
    }
    CRTVector e1, e3;
    e1 = cross(ray.direction, normalVector).normalize();
    e3 = cross(e1, normalVector);
    CRTMatrix localHit(e1, normalVector, e3);
    for (int i = 0; i < GI_RAYS; i++) {
        CRTVector randomDirection = randomHemisphereSample(normalVector);
        // Importance sampling towards light sources
        CRTVector lightDirection = (scene->getRandomLight().getPosition() - data.hitPoint).normalize();
        if (randFloat() < LIGHT_IMPORTANCE) {
            randomDirection = ((1 - LIGHT_IMPORTANCE) * randomDirection + LIGHT_IMPORTANCE * lightDirection);
        }
        CRTRay diffuseReflectionRay{ data.hitPoint + (normalVector * REFLECTION_BIAS),
            randomDirection,
            RayType::REFLECTIVE,
            ray.depth + 1
        };
        diffuseReflections += shade(diffuseReflectionRay, rayTraceAccelerated(diffuseReflectionRay)).clamp(0, 1);
    }
    return diffuseReflections;
}

CRTVector CRTRaytracer::shadeReflective(const CRTRay& ray, const Intersection& data) const {
    CRTVector normalVector = data.faceNormal;
    const CRTMaterial& material = scene->getMaterial(data.materialIndex);
    if (material.smoothShading) {
        normalVector = data.smoothNormal;
    }
    CRTVector reflectedDirection = reflect(ray.direction, normalVector);
    CRTRay reflectedRay{ data.hitPoint + normalVector * REFLECTION_BIAS, reflectedDirection, RayType::REFLECTIVE, ray.depth + 1 };

    return shade(reflectedRay, rayTraceAccelerated(reflectedRay))
        * scene->getGeometryObject(data.hitObjectIndex).sampleMaterial(scene->getMaterial(data.materialIndex), data);
}

CRTVector CRTRaytracer::shadeRefractive(const CRTRay& ray, const Intersection& data) const {
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
        return shade(reflectedRay, rayTraceAccelerated(reflectedRay));
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

    return fresnel * shade(reflectedRay, rayTraceAccelerated(reflectedRay)) 
        + (1 - fresnel) * shade(refractedRay, rayTraceAccelerated(refractedRay));
}
