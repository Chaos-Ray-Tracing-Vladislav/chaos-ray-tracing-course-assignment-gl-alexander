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
		static_cast<float>(arr[0].GetFloat()),
		static_cast<float>(arr[1].GetFloat()),
		static_cast<float>(arr[2].GetFloat())
	};
	return vec;
}

CRTMatrix CRTSceneFactory::loadMatrix(const Value::ConstArray& arr) {
	assert(arr.Size() == 9);
	float matrix[3][3] = {
		{
			arr[0].GetFloat(),
			arr[1].GetFloat(),
			arr[2].GetFloat()
		},
		{
			static_cast<float>(arr[3].GetFloat()),
			static_cast<float>(arr[4].GetFloat()),
			static_cast<float>(arr[5].GetFloat())
		},
		{
			static_cast<float>(arr[6].GetFloat()),
			static_cast<float>(arr[7].GetFloat()),
			static_cast<float>(arr[8].GetFloat())
		}
	};
	return CRTMatrix(matrix);
}

CRTLight CRTSceneFactory::loadLight(const Value::ConstObject& lightVal) {
	const Value& intensityVal = lightVal.FindMember(crtSceneLightIntensity)->value;
	assert(!intensityVal.IsNull() && intensityVal.IsNumber());
	float intensityResult = intensityVal.GetFloat();
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
		//const Value& sceneVersion = settingsVal.FindMember(crtSceneVersion)->value;
		//if (!sceneVersion.IsNull() && sceneVersion.IsInt()) {
		//	settings.version = sceneVersion.GetInt();
		//}

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

			const Value& imageBucketSize = imageSettingsVal.FindMember(crtSceneImageBucketSize)->value;
			if (!imageBucketSize.IsNull() && imageBucketSize.IsInt()) {
				settings.imageSettings.bucketSize = imageBucketSize.GetInt();
			}

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

std::vector<CRTVector> CRTSceneFactory::loadVertices(const Value::ConstArray& arr, CRTBox& AABB) {
	size_t verticesCount = arr.Size() / CRTVector::MEMBERS_COUNT;
	std::vector<CRTVector> result;
	assert(arr.Size() % CRTVector::MEMBERS_COUNT == 0);
	for (int i = 0; i < verticesCount; i++) {
		size_t currentBatch = i * CRTVector::MEMBERS_COUNT;
		CRTVector vec{
		static_cast<float>(arr[currentBatch + 0].GetFloat()),
		static_cast<float>(arr[currentBatch + 1].GetFloat()),
		static_cast<float>(arr[currentBatch + 2].GetFloat())
		};
		AABB.max.x = std::max(vec.x, AABB.max.x);
		AABB.max.y = std::max(vec.y, AABB.max.y);
		AABB.max.z = std::max(vec.z, AABB.max.z);

		AABB.min.x = std::min(vec.x, AABB.min.x);
		AABB.min.y = std::min(vec.y, AABB.min.y);
		AABB.min.z = std::min(vec.z, AABB.min.z);
		result.push_back(vec);
	}
	return result;
}

std::vector<CRTVector> CRTSceneFactory::loadUVVertices(const rapidjson::Value::ConstArray& arr)
{
	size_t verticesCount = arr.Size() / CRTVector::MEMBERS_COUNT;
	std::vector<CRTVector> result;
	assert(arr.Size() % CRTVector::MEMBERS_COUNT == 0);
	for (int i = 0; i < verticesCount; i++) {
		size_t currentBatch = i * CRTVector::MEMBERS_COUNT;
		CRTVector vec{
		static_cast<float>(arr[currentBatch + 0].GetFloat()),
		static_cast<float>(arr[currentBatch + 1].GetFloat()),
		static_cast<float>(arr[currentBatch + 2].GetFloat())
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

CRTMesh CRTSceneFactory::loadMesh(const Value::ConstObject& meshVal, CRTBox& AABB) {
	const Value& meshVertices = meshVal.FindMember(crtSceneVertices)->value;
	assert(!meshVertices.IsNull() && meshVertices.IsArray());

	const Value& triangleVal = meshVal.FindMember(crtSceneTriangles)->value;
	assert(!triangleVal.IsNull() && triangleVal.IsArray());

	const Value& materialIndexVal = meshVal.FindMember(crtSceneMeshMaterialIndex)->value;
	assert(!materialIndexVal.IsNull() && materialIndexVal.IsInt());

	const Value& meshUVs = meshVal.FindMember(crtSceneUVs)->value;
	if (!meshUVs.IsNull() && meshUVs.IsArray()) {
		return CRTMesh(loadVertices(meshVertices.GetArray(), AABB),
			loadUVVertices(meshUVs.GetArray()),
			loadTriangleIndices(triangleVal.GetArray()),
			materialIndexVal.GetInt());
	}
	return CRTMesh(loadVertices(meshVertices.GetArray(), AABB),
		loadTriangleIndices(triangleVal.GetArray()),
		materialIndexVal.GetInt());
}

std::vector<CRTMesh> CRTSceneFactory::parseObjects(const Document& doc, CRTBox& AABB) {
	std::vector<CRTMesh> geometryObjects;
	const Value& objectsVal = doc.FindMember(crtSceneObjects)->value;
	if (!objectsVal.IsNull() && objectsVal.IsArray()) {
		size_t objectsCount = objectsVal.GetArray().Size();
		for (int i = 0; i < objectsCount; i++) {
			assert(!objectsVal.GetArray()[i].IsNull() && objectsVal.GetArray()[i].IsObject());
			geometryObjects.push_back(loadMesh(objectsVal.GetArray()[i].GetObject(), AABB));
		}
	}
	return geometryObjects;
}

CRTMaterial CRTSceneFactory::loadMaterial(const Value::ConstObject& matVal) {
	CRTMaterial material;
	const Value& typeVal = matVal.FindMember(crtSceneMeshMaterialType)->value;
	assert(!typeVal.IsNull() && typeVal.IsString());
	const char* typeAsString = typeVal.GetString();
	if (strcmp(typeAsString, crtSceneMeshMaterialDiffuse) == 0) {
		material.type = CRTMaterialType::DIFFUSE;
	}
	else if (strcmp(typeAsString, crtSceneMeshMaterialReflective) == 0) {
		material.type = CRTMaterialType::REFLECTIVE;
	}
	else if (strcmp(typeAsString, crtSceneMeshMaterialRefractive) == 0) {
		material.type = CRTMaterialType::REFRACTIVE;
	}
	else {
		throw std::logic_error("material type unknown");
	}
	const Value& shadingVal = matVal.FindMember(crtSceneMeshMaterialSmoothShading)->value;
	assert(!shadingVal.IsNull() && shadingVal.IsBool());
	material.smoothShading = shadingVal.GetBool();

	const Value& textureNameVal = matVal.FindMember(crtSceneMeshTextureName)->value;
	if (!textureNameVal.IsNull() && textureNameVal.IsArray()) {
		material.albedo = loadVector(textureNameVal.GetArray());
		material.constantAlbedo = true;
	}
	else if (!textureNameVal.IsNull() && textureNameVal.IsString()) {
		material.textureName = textureNameVal.GetString();
		material.constantAlbedo = false;
	}

	if (material.type == CRTMaterialType::REFRACTIVE) {
		const Value& iorVal = matVal.FindMember(crtSceneMeshMaterialIndexOfRefraction)->value;
		assert(!iorVal.IsNull() && iorVal.IsDouble());
		material.ior = iorVal.GetFloat();
	}

	return material;
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

	CRTBox AABB;
	std::vector<CRTLight> lights = parseLights(doc);
	std::vector<std::shared_ptr<Texture>> textures = CRTTextureFactory::parseTextures(doc);
	std::vector<CRTMaterial> materials = parseMaterials(doc);
	std::vector<CRTMesh> geometryObjects = parseObjects(doc, AABB);

	return new CRTScene(camera, settings, geometryObjects, materials, textures, lights, AABB);
}
