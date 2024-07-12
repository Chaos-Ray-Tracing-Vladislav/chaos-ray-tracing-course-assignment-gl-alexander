#include "CRTScene.h"
#include <stdexcept>

CRTScene::CRTScene(const CRTCamera& camera, const CRTSettings& settings,
	const std::vector<CRTMesh>& geometryObjects, const std::vector<CRTMaterial>& materials,
	const std::unordered_map<std::string, std::shared_ptr<Texture>>& textures, const std::vector<CRTLight>& lights, const CRTBox& AABB)
	: camera(camera), settings(settings), geometryObjects(geometryObjects), materials(materials), lights(lights), textures(textures), AABB(AABB)
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
	return textures.at(textureName);
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

const CRTBox& CRTScene::getAABB() const
{
	return AABB;
}
