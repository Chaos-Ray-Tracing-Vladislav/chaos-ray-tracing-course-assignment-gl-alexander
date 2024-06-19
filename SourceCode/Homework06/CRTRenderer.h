#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include "Utils/CRTTriangle.h"
#include "Utils/CRTColor.h"
#include "CRTScreen.h"

using CRTScene = std::vector<CRTTriangle>;
using CRTDistanceScene = std::vector<std::pair<float, const CRTTriangle*>>;
using CRTImage = std::vector<std::vector<CRTColor>>;

class CRTRenderer
{
	CRTScreen screen;

	CRTDistanceScene sortScene(const CRTScene& scene) const;
public:
	CRTRenderer(const CRTScreen& screen);

	CRTImage renderScene(const CRTScene& scene, std::function<CRTColor(const CRTVector&, const CRTTriangle&, float, const CRTVector&)> visualization) const;
};

CRTColor visualizeByDepth(const CRTVector& point, const CRTTriangle& triangle, float maxDistance, const CRTVector& cameraPosition);
CRTColor visualizeByTriangle(const CRTVector& point, const CRTTriangle& triangle, float maxDistance, const CRTVector& cameraPosition);

