#pragma once
#include <vector>
#include <string>
#include "CRTCamera.h"
#include "CRTMesh.h"
#include "CRTSettings.h"
#include "CRTLight.h"


class CRTScene
{
	std::vector<CRTMesh> geometryObjects;
	std::vector<CRTMaterial> materials;
	CRTCamera camera;
	CRTSettings settings;
	std::vector<CRTLight> lights;
public:
	CRTScene(const CRTCamera& camera, const CRTSettings& settings, 
		const std::vector<CRTMesh>& geometryObjects, const std::vector<CRTMaterial>& materials, 
		const std::vector<CRTLight>& lights);
	
	const CRTMesh& getGeometryObject(int index) const;
	const CRTMaterial& getMaterial(int index) const;
	int getObjectsCount() const;

	const CRTCamera& getCamera() const;
	const CRTSettings& getSettings() const;
	const std::vector<CRTLight>& getLights() const;
};
