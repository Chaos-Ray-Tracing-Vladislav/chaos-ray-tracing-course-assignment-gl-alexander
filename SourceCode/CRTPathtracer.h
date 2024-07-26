#pragma once
#include "CRTRaytracer.h"

constexpr float MIN_INTENSITY = 0.001f;

struct PathVertex2 {
	Intersection intersection;
	CRTVector color;
	float pdf;
};

struct PathVertex {
	CRTVector point;
	CRTVector normal;
	CRTVector w_i;
	CRTVector w_o;
	CRTVector color;
	float pdf;
};

constexpr int CAM_PATH_LENGHT = 5;
constexpr int LIGHT_PATH_LENGHT = 5;

class CRTPathtracer : public CRTRaytracer
{

	CRTVector pathTrace(const CRTRay& ray, const CRTVector& pathMultiplier) const;
	void pathTrace(const CRTRay& ray, const CRTVector& pathMultiplier, std::vector<PathVertex>& path) const;
	void spawnRay(const Intersection& data, const CRTVector& vec_in,
		CRTRay& ray_out, CRTVector& color_out, float& probability) const;
	void spawnDiffuseRay(const Intersection& data, const CRTVector& vec_in,
		CRTRay& ray_out, CRTVector& color_out, float& probability) const;
	void spawnReflectRay(const Intersection& data, const CRTVector& vec_in,
		CRTRay& ray_out, CRTVector& color_out, float& probability) const;
	void spawnRefractRay(const Intersection& data, const CRTVector& vec_in,
		CRTRay& ray_out, CRTVector& color_out, float& probability) const;

	bool connected(const CRTVector& a, const CRTVector& b) const;

	std::vector<PathVertex> getLigthPath(const CRTLight& light) const;
	std::vector<PathVertex> getCameraPath(const CRTRay& ray) const;

	void renderRegion(int x, int y, int width, int height, CRTImage& output) const;

	// TODO: FIXXXX
	CRTVector computeColor(const CRTRay& cameraRay, CRTImage& image) const;
	CRTVector directIllumination(const PathVertex& data, const CRTLight& light) const;
	CRTVector computePathColor(const std::vector<PathVertex>& cameraPath, int cameraNode, const std::vector<PathVertex>& lightPath, int lightNode) const;
public:
	CRTPathtracer(CRTScene* scene);

	void renderScene(const char* outputname) const;
	CRTImage renderScene() const;
};

