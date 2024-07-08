#pragma once
#include <fstream>
#include "CRTScene.h"
#include "Textures/CRTTextureFactory.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

class CRTSceneFactory {
	static CRTVector loadVector(const rapidjson::Value::ConstArray& arr);
	static CRTMatrix loadMatrix(const rapidjson::Value::ConstArray& arr);
	static CRTLight loadLight(const rapidjson::Value::ConstObject& lightVal);

	static std::vector<CRTVector> loadVertices(const rapidjson::Value::ConstArray& arr);
	static std::vector<int> loadTriangleIndices(const rapidjson::Value::ConstArray& arr);
	static CRTMesh loadMesh(const rapidjson::Value::ConstObject& meshVal);
	static CRTMaterial loadMaterial(const rapidjson::Value::ConstObject& matVal);

	static std::vector<CRTLight> parseLights(const rapidjson::Document& doc);
	static void parseSettings(const rapidjson::Document& doc, CRTSettings& settings, CRTCamera& camera);
	static std::vector<CRTMesh> parseObjects(const rapidjson::Document& doc);
	static std::vector<CRTMaterial> parseMaterials(const rapidjson::Document& doc);

public: 
	static CRTScene* factory(const char* filename);
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
static const char* crtSceneUVs = "uvs";
static const char* crtSceneTriangles = "triangles";
static const char* crtSceneLights = "lights";
static const char* crtSceneLightPosition = "position";
static const char* crtSceneLightIntensity = "intensity";
static const char* crtSceneMeshMaterials = "materials";
static const char* crtSceneMeshMaterialType = "type";
static const char* crtSceneMeshTextureName = "albedo";
static const char* crtSceneMeshMaterialSmoothShading = "smooth_shading";
static const char* crtSceneMeshMaterialIndex = "material_index";
static const char* crtSceneMeshMaterialDiffuse = "diffuse";
static const char* crtSceneMeshMaterialReflective = "reflective";
static const char* crtSceneMeshMaterialRefractive = "refractive";
static const char* crtSceneMeshMaterialIndexOfRefraction = "ior";

