#include "CRTTextureFactory.h"
using namespace rapidjson;

static CRTVector loadVector(const Value::ConstArray& arr) {
	assert(arr.Size() == 3);
	CRTVector vec{
		static_cast<float>(arr[0].GetDouble()),
		static_cast<float>(arr[1].GetDouble()),
		static_cast<float>(arr[2].GetDouble())
	};
	return vec;
}

std::shared_ptr<Texture> CRTTextureFactory::loadTexture(const rapidjson::Value::ConstObject& textureVal)
{
	const Value& typeVal = textureVal.FindMember(crtSceneTextureType)->value;
	assert(!typeVal.IsNull() && typeVal.IsString());
	std::string type = typeVal.GetString();
	if (type == crtSceneTextureAlbedo) {
		return loadAlbedo(textureVal);
	}
	else if (type == crtSceneTextureEdges) {
		return loadEdges(textureVal);
	}
	else if (type == crtSceneTextureChecker) {
		return loadChecker(textureVal);
	}
	else if (type == crtSceneTextureBitmap) {
		return loadBitmap(textureVal);
	}
	else {
		assert(false);
	}
	return nullptr;
}

std::shared_ptr<Texture> CRTTextureFactory::loadAlbedo(const rapidjson::Value::ConstObject& textureObj)
{
	const Value& nameVal = textureObj.FindMember(crtSceneTextureName)->value;
	assert(!nameVal.IsNull() && nameVal.IsString());

	const Value& albedoVal = textureObj.FindMember(crtSceneTextureAlbedoVector)->value;
	assert(!albedoVal.IsNull() && albedoVal.IsArray());
	return std::make_shared<ConstantTexture>(nameVal.GetString(), loadVector(albedoVal.GetArray()));
}

std::shared_ptr<Texture> CRTTextureFactory::loadEdges(const rapidjson::Value::ConstObject& textureObj)
{
	const Value& nameVal = textureObj.FindMember(crtSceneTextureName)->value;
	assert(!nameVal.IsNull() && nameVal.IsString());

	const Value& colorEdge = textureObj.FindMember(crtSceneTextureEdgeColorEdge)->value;
	assert(!colorEdge.IsNull() && colorEdge.IsArray());

	const Value& colorInner = textureObj.FindMember(crtSceneTextureEdgeColorInner)->value;
	assert(!colorInner.IsNull() && colorInner.IsArray());

	const Value& edgeWidth = textureObj.FindMember(crtSceneTextureEdgeWidth)->value;
	assert(!edgeWidth.IsNull() && edgeWidth.IsFloat());
	return std::make_shared<EdgeTexture>(nameVal.GetString(), 
		loadVector(colorEdge.GetArray()), 
		loadVector(colorInner.GetArray()), 
		edgeWidth.GetFloat());
}

std::shared_ptr<Texture> CRTTextureFactory::loadChecker(const rapidjson::Value::ConstObject& textureObj)
{
	const Value& nameVal = textureObj.FindMember(crtSceneTextureName)->value;
	assert(!nameVal.IsNull() && nameVal.IsString());

	const Value& colorA = textureObj.FindMember(crtSceneTextureCheckerColorA)->value;
	assert(!colorA.IsNull() && colorA.IsArray());

	const Value& colorB = textureObj.FindMember(crtSceneTextureCheckerColorB)->value;
	assert(!colorB.IsNull() && colorB.IsArray());

	const Value& squareSize = textureObj.FindMember(crtSceneTextureCheckerSquareSize)->value;
	assert(!squareSize.IsNull() && squareSize.IsFloat());
	return std::make_shared<CheckerTexture>(nameVal.GetString(),
		loadVector(colorA.GetArray()),
		loadVector(colorB.GetArray()),
		squareSize.GetFloat());
}

std::shared_ptr<Texture> CRTTextureFactory::loadBitmap(const rapidjson::Value::ConstObject& textureObj)
{
	const Value& nameVal = textureObj.FindMember(crtSceneTextureName)->value;
	assert(!nameVal.IsNull() && nameVal.IsString());

	const Value& pathToImage = textureObj.FindMember(crtSceneTextureBitmapPath)->value;
	assert(!pathToImage.IsNull() && pathToImage.IsString());

	return std::make_shared<BitmapTexture>(nameVal.GetString(), pathToImage.GetString());
}

std::vector<std::shared_ptr<Texture>> CRTTextureFactory::parseTextures(const rapidjson::Document& doc)
{
	std::vector<std::shared_ptr<Texture>> result;
	const Value& objectsVal = doc.FindMember(crtSceneTextures)->value;
	if (!objectsVal.IsNull() && objectsVal.IsArray()) {
		size_t objectsCount = objectsVal.GetArray().Size();
		for (int i = 0; i < objectsCount; i++) {
			assert(!objectsVal.GetArray()[i].IsNull() && objectsVal.GetArray()[i].IsObject());
			result.push_back(loadTexture(objectsVal.GetArray()[i].GetObject()));
		}
	}
	return result;
}