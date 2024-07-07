#include "CRTScene.h"

CRTScene::CRTScene(const CRTCamera& camera, const CRTSettings& settings,
	const std::vector<CRTMesh>& geometryObjects, const std::vector<CRTMaterial>& materials,
	const std::vector<std::shared_ptr<Texture>>& textures, const std::vector<CRTLight>& lights)
	: camera(camera), settings(settings), geometryObjects(geometryObjects), materials(materials), lights(lights), textures(textures)
{}

int CRTScene::getObjectsCount() const
{
	return geometryObjects.size();
}

const CRTMesh& CRTScene::getGeometryObject(int index) const
{
	return geometryObjects[index];
}

const CRTMaterial& CRTScene::getMaterial(int index) const {
	return materials[index];
}

std::shared_ptr<Texture> CRTScene::getTexture(const std::string& textureName) const
{
	for (auto texture : textures) {
		if (texture->name == textureName) {
			return texture;
		}
	}
	return nullptr;
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
