#pragma once
#include <iostream>
#include <vector>
#include "Utils/CRTTriangle.h"
#include "Scene/CRTScene.h"
#include "Utils/CRTImageSaver.h"

using CRTImage = std::vector<std::vector<CRTVector>>;

constexpr int MAX_RAY_DEPTH = 3;
constexpr float PI = 3.1415f;
constexpr float SHADOW_BIAS = 0.001f;
constexpr float REFLECTION_BIAS = 0.001f;

class CRTRenderer
{
	const CRTScene* scene;

	Intersection rayTrace(const CRTRay& ray) const;

	CRTVector shade(const CRTRay& ray, const Intersection& data) const;
	CRTVector shadeDiffuse(const CRTRay& ray, const Intersection& data) const;
	CRTVector shadeReflective(const CRTRay& ray, const Intersection& data) const;
	bool intersectsObject(const CRTRay& ray) const;
public:
	CRTRenderer(const CRTScene* scene);

	void renderScene(const char* outputname) const;
	void renderSceneBarycentic(const char* outputname) const;
	CRTImage renderScene() const;
	CRTImage renderSceneBarycentic() const;
};
