#pragma once
#include <vector>
#include <string>
#include "CRTCamera.h"
#include "CRTMesh.h"
#include "CRTSettings.h"
#include "CRTLight.h"
#include "../rapidjson/document.h"
#include "../rapidjson/istreamwrapper.h"

static const char* crtSceneSettings = "settings";
static const char* crtSceneBGColor = "background_color";
static const char* crtSceneImageSettings = "image_settings";
static const char* crtSceneImageWidth = "width";
static const char* crtSceneImageHeight = "height";
static const char* crtSceneCamera = "camera";
static const char* crtSceneCameraMatrix = "matrix";
static const char* crtSceneCameraPosition = "position";
static const char* crtSceneObjects = "objects";
static const char* crtSceneVertices = "vertices";
static const char* crtSceneTriangles = "triangles";
static const char* crtSceneLights = "lights";
static const char* crtSceneLightPosition = "position";
static const char* crtSceneLightIntensity = "intensity";


class CRTScene
{
	std::vector<CRTMesh> geometryObjects;
	CRTCamera camera;
	CRTSettings settings;
	std::vector<CRTLight> lights;

	void parseLights(const rapidjson::Document& doc);
	void parseSettings(const rapidjson::Document& doc);
	void parseObjects(const rapidjson::Document& doc);
public:
	CRTScene(const CRTCamera& camera, const CRTSettings& settings);
	CRTScene(const CRTCamera& camera, const CRTSettings& settings, const std::vector<CRTTriangle>& triangles);
	CRTScene(const char* filename);

	void addTriangle(const CRTTriangle& triangle);

	const std::vector<CRTMesh>& getGeometryObjects() const;
	const CRTCamera& getCamera() const;
	const CRTSettings& getSettings() const;
	const std::vector<CRTLight>& getLights() const;

	void parseSceneFile(const char* filename);
};

