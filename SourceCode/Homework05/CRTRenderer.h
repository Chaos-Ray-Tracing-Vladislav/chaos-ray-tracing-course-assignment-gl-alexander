#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include "Utils/CRTTriangle.h"
#include "Utils/CRTColor.h"
#include "Utils/CRTScreen.h"

using CRTScene = std::vector<CRTTriangle>;
using CRTImage = std::vector<std::vector<CRTColor>>;

class CRTRenderer
{
	CRTScreen screen;
	CRTVector cameraPosition;
public:
	CRTRenderer(const CRTScreen& screen, const CRTVector& cameraPosition);

	CRTImage renderScene(const CRTScene& scene, std::function<CRTColor(const CRTVector&, const CRTTriangle&, float)> visualization) const;
};

