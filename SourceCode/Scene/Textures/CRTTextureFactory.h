#pragma once
#include <unordered_map>
#include <memory>
#include "Texture.h"
#include "../rapidjson/document.h"

class CRTTextureFactory
{
	static void loadTexture(const rapidjson::Value::ConstObject& textureObj, 
		std::unordered_map<std::string, std::shared_ptr<Texture>>& textures);
	static std::shared_ptr<Texture> loadAlbedo(const rapidjson::Value::ConstObject& textureObj);
	static std::shared_ptr<Texture> loadEdges(const rapidjson::Value::ConstObject& textureObj);
	static std::shared_ptr<Texture> loadChecker(const rapidjson::Value::ConstObject& textureObj);
	static std::shared_ptr<Texture> loadBitmap(const rapidjson::Value::ConstObject& textureObj);
public:
	static std::unordered_map<std::string, std::shared_ptr<Texture>> parseTextures(const rapidjson::Document& doc);
};

static const char* crtSceneTextures = "textures";
static const char* crtSceneTextureName = "name";
static const char* crtSceneTextureType = "type";
static const char* crtSceneTextureAlbedo = "albedo";
static const char* crtSceneTextureAlbedoVector = "albedo";
static const char* crtSceneTextureEdges = "edges";
static const char* crtSceneTextureEdgeColorEdge = "edge_color";
static const char* crtSceneTextureEdgeColorInner = "inner_color";
static const char* crtSceneTextureEdgeWidth = "edge_width";
static const char* crtSceneTextureChecker = "checker";
static const char* crtSceneTextureCheckerColorA = "color_A";
static const char* crtSceneTextureCheckerColorB = "color_B";
static const char* crtSceneTextureCheckerSquareSize = "square_size";
static const char* crtSceneTextureBitmap = "bitmap";
static const char* crtSceneTextureBitmapPath = "file_path";
