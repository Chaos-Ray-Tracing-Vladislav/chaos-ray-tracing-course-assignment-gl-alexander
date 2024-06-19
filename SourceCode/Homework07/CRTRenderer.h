#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include "Utils/CRTTriangle.h"
#include "Utils/CRTColor.h"
#include "Scene/CRTScene.h"
#include "Utils/CRTImageSaver.h"

using CRTDistanceScene = std::vector<std::pair<float, const CRTTriangle*>>;
using CRTImage = std::vector<std::vector<CRTColor>>;

class CRTRenderer
{
	const CRTScene* scene;

	std::vector<CRTTriangle> convertMeshesToTriangles() const;
	float getMaxDistanceToCamera(const std::vector<CRTTriangle>& triangles) const;
public:
	CRTRenderer(const CRTScene* scene);

	void renderScene(const char* outputname) const;
	CRTImage renderScene() const;
	CRTImage renderScene(std::function<CRTColor(const CRTVector&, const CRTTriangle&, float, const CRTVector&)> visualization) const;
};

CRTColor visualizeByDepth(const CRTVector& point, const CRTTriangle& triangle, float maxDistance, const CRTVector& cameraPosition);
CRTColor visualizeByTriangle(const CRTVector& point, const CRTTriangle& triangle, float maxDistance, const CRTVector& cameraPosition);

