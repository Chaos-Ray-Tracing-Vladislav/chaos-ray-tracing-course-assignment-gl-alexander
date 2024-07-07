#pragma once
#include <vector>
#include <memory>
#include "ConstantTexture.h"
#include "EdgeTexture.h"
#include "CheckerTexture.h"
#include "BitmapTexture.h"
#include "../../rapidjson/document.h"

class CRTTextureFactory
{
	static std::shared_ptr<Texture> loadTexture(const rapidjson::Value::ConstObject& textureObj);
	static std::shared_ptr<Texture> loadAlbedo(const rapidjson::Value::ConstObject& textureObj);
	static std::shared_ptr<Texture> loadEdges(const rapidjson::Value::ConstObject& textureObj);
	static std::shared_ptr<Texture> loadChecker(const rapidjson::Value::ConstObject& textureObj);
	static std::shared_ptr<Texture> loadBitmap(const rapidjson::Value::ConstObject& textureObj);
public:
	static std::vector<std::shared_ptr<Texture>> parseTextures(const rapidjson::Document& doc);
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
