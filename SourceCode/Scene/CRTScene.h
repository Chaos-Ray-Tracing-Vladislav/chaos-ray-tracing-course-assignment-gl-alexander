#pragma once
#include <vector>
#include <memory>
#include <string>
#include "CRTCamera.h"
#include "CRTMesh.h"
#include "CRTSettings.h"
#include "CRTLight.h"
#include "Textures/Texture.h"
#include "../Utils/CRTBox.h"

class CRTScene
{
	CRTBox AABB;
	std::vector<CRTMesh> geometryObjects;
	std::vector<CRTMaterial> materials;
	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

	CRTCamera camera;
	CRTSettings settings;
	std::vector<CRTLight> lights;

	std::shared_ptr<Texture> getTexture(const std::string& textureName) const;

public:
	CRTScene(const CRTCamera& camera, const CRTSettings& settings, 
		const std::vector<CRTMesh>& geometryObjects, const std::vector<CRTMaterial>& materials, 
		const std::unordered_map<std::string, std::shared_ptr<Texture>>& textures, const std::vector<CRTLight>& lights,
		const CRTBox& AABB);
	
	const CRTMesh& getGeometryObject(int index) const;
	const CRTMaterial& getMaterial(int index) const;
	int getObjectsCount() const;

	const CRTCamera& getCamera() const;
	const CRTSettings& getSettings() const;
	const std::vector<CRTLight>& getLights() const;
	const CRTBox& getAABB() const;
};
