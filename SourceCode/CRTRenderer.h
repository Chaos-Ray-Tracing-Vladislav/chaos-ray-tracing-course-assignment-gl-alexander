#pragma once
#include <iostream>
#include <vector>
#include "Utils/CRTTriangle.h"
#include "Scene/CRTScene.h"
#include "Utils/CRTImageSaver.h"

using CRTDistanceScene = std::vector<std::pair<float, const CRTTriangle*>>;
using CRTImage = std::vector<std::vector<CRTColor>>;
typedef CRTColor (*VisualizationFunction)(const CRTVector&, const CRTVector&);

constexpr float MAX_RENDER_DISTANCE = 10.0f;
constexpr float PI = 3.1415f;
constexpr float SHADOW_BIAS = 0.2f;
const CRTColor ALBEDO(240, 200, 200);

class CRTRenderer
{
	const CRTScene* scene;

	CRTColor shade(const CRTVector& point, const CRTTriangle& triangleHit) const;
	bool intersectsObject(const CRTRay& ray) const;
public:
	CRTRenderer(const CRTScene* scene);

	void renderScene(const char* outputname) const;
	CRTImage renderScene() const;
};

CRTColor visualizeByDepth(const CRTVector& point, const CRTVector& cameraPosition);

