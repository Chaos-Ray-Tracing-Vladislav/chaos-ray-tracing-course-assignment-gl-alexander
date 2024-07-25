#pragma once
#include "Utils/CRTVector.h"
#include "Scene/CRTScene.h"
#include "Utils/CRTImageSaver.h"
#include "Scene/CRTBucketArray.h"
#include "Utils/FXAA.h"

//using CRTImage = std::vector<std::vector<CRTVector>>;

constexpr int MAX_RAY_DEPTH = 4;
constexpr float SHADOW_BIAS = 0.001f;
constexpr float REFLECTION_BIAS = 0.001f;
constexpr float REFRACTION_BIAS = 0.001f;

static const float DESATURIZATION = 0.75;

constexpr int GI_RAYS = 1;
constexpr float LIGHT_IMPORTANCE = 0.6;

class CRTRaytracer
{
protected: 
	CRTScene* scene;

	Intersection rayTrace(const CRTRay& ray) const;
	Intersection rayTraceAccelerated(const CRTRay& ray, float maxDistance = FLOAT_MAX) const;

	CRTVector shade(const CRTRay& ray, const Intersection& data) const;
	CRTVector shadeDiffuse(const CRTRay& ray, const Intersection& data) const;
	CRTVector shadeDirectIllumination(const CRTRay& ray, const Intersection& data) const;
	CRTVector shadeGlobalIllumination(const CRTRay& ray, const Intersection& data) const;
	CRTVector shadeReflective(const CRTRay& ray, const Intersection& data) const;
	CRTVector shadeRefractive(const CRTRay& ray, const Intersection& data) const;

	bool intersectsObject(const CRTRay& ray, float distanceToLight) const;

	// old non-accelerated methods
	void renderRegionNoAABB(int x, int y, int width, int height, CRTImage& output) const;
	void renderRegionSimple(int x, int y, int width, int height, CRTImage& output) const;

	void renderRegion(int x, int y, int width, int height, CRTImage& output) const;
	// Picks random points in pixel from which to shoot rays
	void renderRegionMonteCarloAA(int x, int y, int width, int height, CRTImage& output) const;
	// Picks points in pixel in a grid-like patter
	void renderRegionPixelGrid(int x, int y, int width, int height, CRTImage& output) const;
	// Renders the scene simulating a Depth of Field effect
	void renderRegionFocalBlur(int x, int y, int width, int height, CRTImage& output) const;
	// Simulates two rays (resembling human eyes) and renders them 
	void renderRegionStereoscopy(int x, int y, int width, int height, CRTImage& output) const;
	// Combines DOF and Stereoscopic rendering 
	void renderRegionDOFStereoscopy(int x, int y, int width, int height, CRTImage& output) const;



	// for testing purposes
	CRTImage renderSinglethreaded() const;
	CRTImage renderByRegions() const;
	CRTImage renderByBuckets() const;
	CRTImage renderWithAABB() const;
public:
	CRTRaytracer(CRTScene* scene);

	void renderScene(const char* outputname) const;
	CRTImage renderAccelerated() const;

	void renderAnimation(const char* outputname, std::vector<CRTCamera> keyframes) const;

	// for testing purposes:
	void renderSceneBarycentic(const char* outputname) const;
	CRTImage renderSceneBarycentic() const;
	CRTImage renderAcceleratedSinglethreaded() const;

};
