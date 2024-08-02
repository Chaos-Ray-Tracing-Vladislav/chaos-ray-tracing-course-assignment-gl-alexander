#pragma once
#include <fstream>
#include "CRTScene.h"
#include "Textures/CRTTextureFactory.h"
#include "../Dependencies/rapidjson/document.h"
#include "../Dependencies/rapidjson/istreamwrapper.h"

using TextureMap = std::unordered_map<std::string, std::shared_ptr<Texture>>;

class CRTSceneFactory {

	static CRTVector loadVector(const rapidjson::Value::ConstArray& arr);
	static CRTMatrix loadMatrix(const rapidjson::Value::ConstArray& arr);
	static CRTLight loadLight(const rapidjson::Value::ConstObject& lightVal);

	static std::vector<CRTVector> loadVertices(const rapidjson::Value::ConstArray& arr, CRTBox& AABB);
	static std::vector<CRTVector> loadUVVertices(const rapidjson::Value::ConstArray& arr);
	static std::vector<int> loadTriangleIndices(const rapidjson::Value::ConstArray& arr);
	static CRTMesh loadMesh(const rapidjson::Value::ConstObject& meshVal, CRTBox& AABB);
	static CRTMaterial loadMaterial(const rapidjson::Value::ConstObject& matVal, const TextureMap& textures);

	static std::vector<CRTLight> parseLights(const rapidjson::Document& doc);
	static void parseSettings(const rapidjson::Document& doc, CRTSettings& settings, CRTCamera& camera);
	static std::vector<CRTMesh> parseObjects(const rapidjson::Document& doc, CRTBox& AABB);
	static std::vector<CRTMaterial> parseMaterials(const rapidjson::Document& doc, const TextureMap& textures);

public: 
	static CRTScene* factory(const char* filename);
};


static const char* crtSceneSettings = "settings";
static const char* crtSceneVersion = "version";
static const char* crtSceneBGColor = "background_color";
static const char* crtSceneReflectionsOn = "reflections_on";
static const char* crtSceneRefractionsOn = "refractions_on";
static const char* crtSceneGIOn = "gi_on";
static const char* crtSceneFXAAOn = "fxaa_on";
static const char* crtSceneImageSettings = "image_settings";
static const char* crtSceneImageWidth = "width";
static const char* crtSceneImageHeight = "height";
static const char* crtSceneImageBucketSize = "bucket_size";
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
static const char* crtSceneMeshAlbedo = "albedo";
static const char* crtSceneMeshMaterialSmoothShading = "smooth_shading";
static const char* crtSceneMeshMaterialIndex = "material_index";
static const char* crtSceneMeshMaterialDiffuse = "diffuse";
static const char* crtSceneMeshMaterialReflective = "reflective";
static const char* crtSceneMeshMaterialRefractive = "refractive";
static const char* crtSceneMeshMaterialIndexOfRefraction = "ior";

