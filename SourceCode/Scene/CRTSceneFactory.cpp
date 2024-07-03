#include "CRTSceneFactory.h"
using namespace rapidjson;

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

CRTVector CRTSceneFactory::loadVector(const Value::ConstArray& arr) {
	assert(arr.Size() == 3);
	CRTVector vec{
		static_cast<float>(arr[0].GetDouble()),
		static_cast<float>(arr[1].GetDouble()),
		static_cast<float>(arr[2].GetDouble())
	};
	return vec;
}

CRTMatrix CRTSceneFactory::loadMatrix(const Value::ConstArray& arr) {
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

CRTLight CRTSceneFactory::loadLight(const Value::ConstObject& lightVal) {
	const Value& intensityVal = lightVal.FindMember(crtSceneLightIntensity)->value;
	assert(!intensityVal.IsNull() && intensityVal.IsNumber());
	float intensityResult = intensityVal.GetDouble();
	const Value& positionVal = lightVal.FindMember(crtSceneLightPosition)->value;
	assert(!positionVal.IsNull() && positionVal.IsArray());
	return CRTLight(loadVector(positionVal.GetArray()), intensityResult);
}

std::vector<CRTLight> CRTSceneFactory::parseLights(const rapidjson::Document& doc) {
	std::vector<CRTLight> lights;
	const Value& lightsVal = doc.FindMember(crtSceneLights)->value;
	if (!lightsVal.IsNull() && lightsVal.IsArray()) {
		int numLights = lightsVal.GetArray().Size();
		assert(numLights > 0);
		for (int i = 0; i < numLights; i++) {
			assert(!lightsVal.GetArray()[i].IsNull() && lightsVal.GetArray()[i].IsObject());
			lights.push_back(loadLight(lightsVal.GetArray()[i].GetObject()));
		}
	}
	return lights;
}

void CRTSceneFactory::parseSettings(const Document& doc, CRTSettings& settings, CRTCamera& camera) {
	const Value& settingsVal = doc.FindMember(crtSceneSettings)->value;
	if (!settingsVal.IsNull() && settingsVal.IsObject()) {
		const Value& bgColorValue = settingsVal.FindMember(crtSceneBGColor)->value;
		assert(!bgColorValue.IsNull() && bgColorValue.IsArray());
		settings.bgColor = loadVector(bgColorValue.GetArray());

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

std::vector<CRTVector> CRTSceneFactory::loadVertices(const Value::ConstArray& arr) {
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

std::vector<int> CRTSceneFactory::loadTriangleIndices(const Value::ConstArray& arr) {
	assert(arr.Size() % VERTICES == 0);

	std::vector<int> result;
	for (int i = 0; i < arr.Size(); i++) {
		result.push_back(arr[i].GetInt());
	}
	return result;
}

CRTMesh CRTSceneFactory::loadMesh(const Value::ConstObject& meshVal) {
	const Value& meshVertices = meshVal.FindMember(crtSceneVertices)->value;
	assert(!meshVertices.IsNull() && meshVertices.IsArray());
	const Value& triangleVal = meshVal.FindMember(crtSceneTriangles)->value;
	assert(!triangleVal.IsNull() && triangleVal.IsArray());
	const Value& materialIndexVal = meshVal.FindMember(crtSceneMeshMaterialIndex)->value;
	assert(!materialIndexVal.IsNull() && materialIndexVal.IsInt());
	return CRTMesh(loadVertices(meshVertices.GetArray()),
		loadTriangleIndices(triangleVal.GetArray()),
		materialIndexVal.GetInt());
}

std::vector<CRTMesh> CRTSceneFactory::parseObjects(const Document& doc) {
	std::vector<CRTMesh> geometryObjects;
	const Value& objectsVal = doc.FindMember(crtSceneObjects)->value;
	if (!objectsVal.IsNull() && objectsVal.IsArray()) {
		size_t objectsCount = objectsVal.GetArray().Size();
		for (int i = 0; i < objectsCount; i++) {
			assert(!objectsVal.GetArray()[i].IsNull() && objectsVal.GetArray()[i].IsObject());
			geometryObjects.push_back(loadMesh(objectsVal.GetArray()[i].GetObject()));
		}
	}
	return geometryObjects;
}

CRTMaterial CRTSceneFactory::loadMaterial(const Value::ConstObject& matVal) {
	const Value& typeVal = matVal.FindMember(crtSceneMeshMaterialType)->value;
	assert(!typeVal.IsNull() && typeVal.IsString());
	const char* typeAsString = typeVal.GetString();
	CRTMaterialType type = CRTMaterialType::DIFFUSE;
	if (strcmp(typeAsString, crtSceneMeshMaterialDiffuse) == 0) {
		type = CRTMaterialType::DIFFUSE;
	}
	else if (strcmp(typeAsString, crtSceneMeshMaterialReflective) == 0) {
		type = CRTMaterialType::REFLECTIVE;
	}
	else {
		throw std::logic_error("material type unknown");
	}

	const Value& albedoVal = matVal.FindMember(crtSceneMeshMaterialAlbedo)->value;
	assert(!albedoVal.IsNull() && albedoVal.IsArray());
	CRTVector albedo = loadVector(albedoVal.GetArray());

	const Value& shadingVal = matVal.FindMember(crtSceneMeshMaterialSmoothShading)->value;
	assert(!shadingVal.IsNull() && shadingVal.IsBool());
	return { type, albedo, shadingVal.GetBool() };
}

std::vector<CRTMaterial> CRTSceneFactory::parseMaterials(const rapidjson::Document& doc)
{
	std::vector<CRTMaterial> materials;
	const Value& materialsVal = doc.FindMember(crtSceneMeshMaterials)->value;
	if (!materialsVal.IsNull() && materialsVal.IsArray()) {
		size_t materialsCount = materialsVal.GetArray().Size();
		for (int i = 0; i < materialsCount; i++) {
			assert(!materialsVal.GetArray()[i].IsNull() && materialsVal.GetArray()[i].IsObject());
			materials.push_back(loadMaterial(materialsVal.GetArray()[i].GetObject()));
		}
	}
	return materials;
}


CRTScene* CRTSceneFactory::factory(const char* filename)
{
	Document doc = getJsonDocument(filename);
	CRTCamera camera;
	CRTSettings settings;
	parseSettings(doc, settings, camera);

	std::vector<CRTLight> lights = parseLights(doc);
	std::vector<CRTMaterial> materials = parseMaterials(doc);
	std::vector<CRTMesh> geometryObjects = parseObjects(doc);

	return new CRTScene(camera, settings, geometryObjects, materials, lights);
}
