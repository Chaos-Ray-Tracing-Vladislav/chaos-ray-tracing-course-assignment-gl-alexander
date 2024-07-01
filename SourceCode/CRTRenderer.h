#pragma once
#include <iostream>
#include <vector>
#include "Utils/CRTTriangle.h"
#include "Scene/CRTScene.h"
#include "Utils/CRTImageSaver.h"

using CRTImage = std::vector<std::vector<CRTColor>>;

constexpr float MAX_RENDER_DISTANCE = 10.0f;
constexpr int REFLECTION_DEPTH = 3;
constexpr float PI = 3.1415f;
constexpr float SHADOW_BIAS = 0.1f;

class CRTRenderer
{
	const CRTScene* scene;

	CRTColor rayTrace(const CRTRay& ray, int depth, const CRTVector& addedAlbedo) const;
	CRTColor shade(const CRTVector& point, const CRTVector& normalVector, const CRTVector& albedo) const;
	bool intersectsObject(const CRTRay& ray) const;
public:
	CRTRenderer(const CRTScene* scene);

	void renderScene(const char* outputname) const;
	void renderSceneBarycentic(const char* outputname) const;
	CRTImage renderScene() const;
	CRTImage renderSceneBarycentic() const;
};
