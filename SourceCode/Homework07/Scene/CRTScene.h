#pragma once
#include <vector>
#include <string>
#include "CRTCamera.h"
#include "CRTMesh.h"
#include "CRTSettings.h"
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

class CRTScene
{
	std::vector<CRTMesh> geometryObjects;
	CRTCamera camera;
	CRTSettings settings;

	void parseSettings(const rapidjson::Document& doc);
	void parseObjects(const rapidjson::Document& doc);
public:
	CRTScene(const char* filename);

	const std::vector<CRTMesh>& getGeometryObjects() const;
	const CRTCamera& getCamera() const;
	const CRTSettings& getSettings() const;

	void parseSceneFile(const char* filename);
};

