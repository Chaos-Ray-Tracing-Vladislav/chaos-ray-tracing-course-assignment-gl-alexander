#include "CRTScene.h"
#include <fstream>

using namespace rapidjson;

CRTScene::CRTScene(const char* filename)
{
	parseSceneFile(filename);
}

CRTScene::CRTScene(const CRTCamera& camera, const CRTSettings& settings) : camera(camera), settings(settings)
{}

CRTScene::CRTScene(const CRTCamera & camera, const CRTSettings & settings, const std::vector<CRTTriangle>&triangles) : CRTScene(camera, settings)
{
	for (const CRTTriangle& tr : triangles) {
		addTriangle(tr);
	}
}

void CRTScene::addTriangle(const CRTTriangle& triangle) {
	CRTMesh newMesh(triangle.getVertices(), std::vector<int>({ 0, 1, 2 }));
	geometryObjects.push_back(newMesh);
}


const std::vector<CRTMesh>& CRTScene::getGeometryObjects() const
{
	return geometryObjects;
}

const CRTCamera& CRTScene::getCamera() const
{
	return camera;
}

const CRTSettings& CRTScene::getSettings() const
{
	return settings;
}

const std::vector<CRTLight>& CRTScene::getLights() const {
	return lights;
}

static Document getJsonDocument(const char* filename) {
	std::ifstream ifs(filename);
	if (!ifs.is_open())
	{
		throw std::invalid_argument("couldn't open file");
	}

	IStreamWrapper isw(ifs);

	Document doc;
	doc.ParseStream(isw);
	return doc;
}

static CRTVector loadVector(const Value::ConstArray& arr) {
	assert(arr.Size() == 3);
	CRTVector vec{
		static_cast<float>(arr[0].GetDouble()),
		static_cast<float>(arr[1].GetDouble()),
		static_cast<float>(arr[2].GetDouble())
	};
	return vec;
}

static CRTMatrix loadMatrix(const Value::ConstArray& arr) {
	assert(arr.Size() == 9);
	float matrix[3][3] = {
		{
			arr[0].GetDouble(),
			arr[1].GetDouble(),
			arr[2].GetDouble()
		},
		{
			static_cast<float>(arr[3].GetDouble()),
			static_cast<float>(arr[4].GetDouble()),
			static_cast<float>(arr[5].GetDouble())
		},
		{
			static_cast<float>(arr[6].GetDouble()),
			static_cast<float>(arr[7].GetDouble()),
			static_cast<float>(arr[8].GetDouble())
		}
	};
	return CRTMatrix(matrix);
}

static CRTLight loadLight(const Value::ConstObject& lightVal) {
	const Value& intensityVal = lightVal.FindMember(crtSceneLightIntensity)->value;
	assert(!intensityVal.IsNull() && intensityVal.IsNumber());
	float intensityResult = intensityVal.GetDouble();
	const Value& positionVal = lightVal.FindMember(crtSceneLightPosition)->value;
	assert(!positionVal.IsNull() && positionVal.IsArray());
	return CRTLight(loadVector(positionVal.GetArray()), intensityResult);
}

void CRTScene::parseLights(const rapidjson::Document& doc) {
	const Value& lightsVal = doc.FindMember(crtSceneLights)->value;
	if (!lightsVal.IsNull() && lightsVal.IsArray()) {
		int numLights = lightsVal.GetArray().Size();
		assert( numLights > 0);
		for (int i = 0; i < numLights; i++) {
			assert(!lightsVal.GetArray()[i].IsNull() && lightsVal.GetArray()[i].IsObject());
			lights.push_back(loadLight(lightsVal.GetArray()[i].GetObject()));
		}
	}
}

void CRTScene::parseSettings(const Document& doc) {
	const Value& settingsVal = doc.FindMember(crtSceneSettings)->value;
	if (!settingsVal.IsNull() && settingsVal.IsObject()) {
		const Value& bgColorValue = settingsVal.FindMember(crtSceneBGColor)->value;
		assert(!bgColorValue.IsNull() && bgColorValue.IsArray());
		CRTVector colorVec = loadVector(bgColorValue.GetArray());
		settings.bgColor = { (short)colorVec.x, (short)colorVec.y, (short)colorVec.z };

		const Value& imageSettingsVal = settingsVal.FindMember(crtSceneImageSettings)->value;
		if (!imageSettingsVal.IsNull() && imageSettingsVal.IsObject()) {
			const Value& imageWidthVal = imageSettingsVal.FindMember(crtSceneImageWidth)->value;
			const Value& imageHeightVal = imageSettingsVal.FindMember(crtSceneImageHeight)->value;
			assert(!imageWidthVal.IsNull() && !imageHeightVal.IsNull()
				&& imageWidthVal.IsInt() && imageHeightVal.IsInt());
			settings.imageSettings.width = imageWidthVal.GetInt();
			settings.imageSettings.height = imageHeightVal.GetInt();

			camera.setImageSettings(settings.imageSettings.width, settings.imageSettings.height);
		}

		const Value& cameraVal = doc.FindMember(crtSceneCamera)->value;
		if (!cameraVal.IsNull() && cameraVal.IsObject()) {
			const Value& matrixVal = cameraVal.FindMember(crtSceneCameraMatrix)->value;
			assert(!matrixVal.IsNull() && matrixVal.IsArray());
			camera.setRotation(loadMatrix(matrixVal.GetArray()));

			const Value& positionVal = cameraVal.FindMember(crtSceneCameraPosition)->value;
			assert(!positionVal.IsNull() && positionVal.IsArray());
			camera.setPosition(loadVector(positionVal.GetArray()));
		}
	}
}

static std::vector<CRTVector> loadVertices(const Value::ConstArray& arr) {
	size_t verticesCount = arr.Size() / CRTVector::MEMBERS_COUNT;
	std::vector<CRTVector> result;
	assert(arr.Size() % CRTVector::MEMBERS_COUNT == 0);
	for (int i = 0; i < verticesCount; i++) {
		size_t currentBatch = i * CRTVector::MEMBERS_COUNT;
		CRTVector vec{
		static_cast<float>(arr[currentBatch + 0].GetDouble()),
		static_cast<float>(arr[currentBatch + 1].GetDouble()),
		static_cast<float>(arr[currentBatch + 2].GetDouble())
		};
		result.push_back(vec);
	}
	return result;
}

static std::vector<int> loadTriangleIndices(const Value::ConstArray& arr) {
	assert(arr.Size() % VERTICES == 0);

	std::vector<int> result;
	for (int i = 0; i < arr.Size(); i++) {
		result.push_back(arr[i].GetInt());
	}
	return result;
}

static CRTMesh loadMesh(const Value::ConstObject& meshVal) {
	const Value & meshVertices = meshVal.FindMember(crtSceneVertices)->value;
	assert(!meshVertices.IsNull() && meshVertices.IsArray());
	const Value& triangleVal = meshVal.FindMember(crtSceneTriangles)->value;
	assert(!triangleVal.IsNull() && triangleVal.IsArray());

	return CRTMesh(loadVertices(meshVertices.GetArray()), 
		loadTriangleIndices(triangleVal.GetArray()));
}

void CRTScene::parseObjects(const Document& doc) {
	const Value& objectsVal = doc.FindMember(crtSceneObjects)->value;
	if (!objectsVal.IsNull() && objectsVal.IsArray()) {
		size_t objectsCount = objectsVal.GetArray().Size();
		for (int i = 0; i < objectsCount; i++) {
			assert(!objectsVal.GetArray()[i].IsNull() && objectsVal.GetArray()[i].IsObject());
			geometryObjects.push_back(loadMesh(objectsVal.GetArray()[i].GetObject()));
		}	
	}
}


void CRTScene::parseSceneFile(const char* filename)
{
	Document doc = getJsonDocument(filename);
	parseSettings(doc);
	parseObjects(doc);
	parseLights(doc);
}
