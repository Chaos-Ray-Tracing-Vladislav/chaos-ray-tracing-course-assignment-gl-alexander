#pragma once
#include <vector>
#include <string>
#include "CRTCamera.h"
#include "CRTMesh.h"
#include "CRTSettings.h"
#include "CRTLight.h"
#include "../rapidjson/document.h"
#include "../rapidjson/istreamwrapper.h"

class CRTScene
{
	std::vector<CRTMesh> geometryObjects;
	CRTCamera camera;
	CRTSettings settings;
	std::vector<CRTLight> lights;
	std::vector<CRTMaterial> materials;

	CRTVector loadVector(const rapidjson::Value::ConstArray& arr) const;
	CRTMatrix loadMatrix(const rapidjson::Value::ConstArray& arr) const;
	CRTLight loadLight(const rapidjson::Value::ConstObject& lightVal) const;
	std::vector<CRTVector> loadVertices(const rapidjson::Value::ConstArray& arr) const;
	std::vector<int> loadTriangleIndices(const rapidjson::Value::ConstArray& arr) const;
	CRTMesh loadMesh(const rapidjson::Value::ConstObject& meshVal) const;
	CRTMaterial loadMaterial(const rapidjson::Value::ConstObject& matVal) const;

	void parseLights(const rapidjson::Document& doc);
	void parseSettings(const rapidjson::Document& doc);
	void parseObjects(const rapidjson::Document& doc);
	void parseMaterials(const rapidjson::Document& doc);
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
static const char* crtSceneMeshMaterials = "materials";
static const char* crtSceneMeshMaterialAlbedo = "albedo";
static const char* crtSceneMeshMaterialType = "type";
static const char* crtSceneMeshMaterialSmoothShading = "smooth_shading";
static const char* crtSceneMeshMaterialIndex = "material_index";
static const char* crtSceneMeshMaterialDiffuse = "diffuse";
static const char* crtSceneMeshMaterialReflective = "reflective";