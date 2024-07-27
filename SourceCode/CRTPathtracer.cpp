#include "CRTPathtracer.h"
#include <thread>
CRTPathtracer::CRTPathtracer(CRTScene* scene) : CRTRaytracer(scene)
{
}

void CRTPathtracer::renderScene(const char* outputname) const
{
	CRTImageSaver::saveImage(outputname, renderScene());

}

CRTImage CRTPathtracer::renderScene() const
{
	const unsigned imageHeight = scene->getSettings().imageSettings.height;
	const unsigned imageWidth = scene->getSettings().imageSettings.width;

	CRTImage image(imageHeight, std::vector<CRTVector>(imageWidth, scene->getSettings().bgColor));

	CRTBucketArray buckets(scene); // the constructor generates the regions

	int threadCount = std::thread::hardware_concurrency();
	std::vector<std::jthread> threads;
	//threadCount = 1;
	//renderRegion(435, 632, 100, 100, image);
	//return image;
	for (int i = 0; i < threadCount; i++) {
		threads.push_back(std::jthread([this, &buckets, &image]() {
			CRTRegion region;
			while (buckets.nextRegion(region)) {
				renderRegion(region.startX, region.startY, region.width, region.height, image);
			}
		}));
	}
	return image;
}

void CRTPathtracer::renderRegion(int x, int y, int width, int height, CRTImage& output) const
{
	// same as raytracer's renderRegionPixelGrid
	float mult = 1.0f / scene->getSettings().raysPerPixel;
	int squareCount = scene->getSettings().raysPerPixel - 1;
	int squaresPerSide = sqrt(squareCount);
	float squareCenterX, squareCenterY, side, halfside;
	side = 1.0f / squaresPerSide;
	halfside = side / 2.0f;
	for (int rowId = y; rowId < y + height; rowId++) {
		for (int colId = x; colId < x + width; colId++) {
			CRTVector finalColor(0, 0, 0);
			for (int x1 = 0; x1 < squaresPerSide; x1++) {
				for (int y1 = 0; y1 < squaresPerSide; y1++) {
					squareCenterX = x1 * side + halfside;
					squareCenterY = y1 * side + halfside;
					CRTRay ray = scene->getCamera().getRayForSubpixel(rowId + squareCenterY, colId + squareCenterX);
					finalColor += computeColor(ray, output);
				}
			}
			for (int i = squareCount; i < scene->getSettings().raysPerPixel; i++) {
				// generate the rest using montecarlo
				CRTRay ray = scene->getCamera().getRayForSubpixel(rowId + randFloat(), colId + randFloat());
				finalColor += computeColor(ray, output);
			}
			output[rowId][colId] = (finalColor * mult).clamp(0, 1);
		}
	}
}

static float intensity(const CRTVector& color) {
	return 0.21 * color.x + 0.72 * color.y + 0.07 * color.z;
}

CRTVector CRTPathtracer::pathTrace(const CRTRay& ray, const CRTVector& pathMultiplier) const
{
	if (intensity(pathMultiplier) < MIN_INTENSITY || ray.depth >= MAX_RAY_DEPTH) return CRTVector(0, 0, 0);
	
	Intersection ix = rayTraceAccelerated(ray);
	if (ix.hitObjectIndex == INVALID_IND) return CRTVector(0, 0, 0);
	CRTRay newRay;
	CRTVector color;
	float probability;
	spawnRay(ix, ray.direction, newRay, color, probability);
	if (probability <= 0) return CRTVector(0, 0, 0);

	return pathTrace(newRay, pathMultiplier * color / probability);
}

void CRTPathtracer::pathTrace(const CRTRay& ray, const CRTVector& pathMultiplier, std::vector<PathVertex>& path) const
{
	if (intensity(pathMultiplier) < MIN_INTENSITY || ray.depth >= MAX_RAY_DEPTH) return;

	Intersection ix = rayTraceAccelerated(ray);
	if (ix.hitObjectIndex == INVALID_IND) return;
	CRTRay newRay;
	newRay.depth = ray.depth + 1;
	CRTVector color;
	float probability;
	spawnRay(ix, ray.direction, newRay, color, probability);
	if (probability <= 0) return;
	if (scene->getMaterial(ix.materialIndex).type == CRTMaterialType::DIFFUSE)
		//path.emplace_back(ix, color, probability); // so we only check the diffuse materials in the path
	pathTrace(newRay, pathMultiplier * color / probability, path);
}

void CRTPathtracer::spawnRay(const Intersection& data, const CRTVector& vec_in, CRTRay& ray_out, CRTVector& color_out, float& probability) const
{
	switch (scene->getMaterial(data.materialIndex).type) {
	case CRTMaterialType::DIFFUSE:
		spawnDiffuseRay(data, vec_in, ray_out, color_out, probability);
		break;
	case CRTMaterialType::REFLECTIVE:
		spawnReflectRay(data, vec_in, ray_out, color_out, probability);
		break;
	case CRTMaterialType::REFRACTIVE:
		spawnRefractRay(data, vec_in, ray_out, color_out, probability);
		break;
	default:
		color_out = CRTVector(0, 0, 0);
	}
}

void CRTPathtracer::spawnDiffuseRay(const Intersection& data, const CRTVector& vec_in, CRTRay& ray_out, CRTVector& color_out, float& probability) const
{
	// very similar to the GI algorithm in the Raytrace variant
	const CRTMaterial& material = scene->getMaterial(data.materialIndex);
	CRTVector normal = data.faceNormal;
	if (material.smoothShading) normal = data.smoothNormal;

	ray_out.direction = randomHemisphereSample(normal);
	ray_out.origin = data.hitPoint + normal * REFLECTION_BIAS;

	CRTVector rawColor = scene->getGeometryObject(data.hitObjectIndex).sampleMaterial(material, data);
	color_out = rawColor * (1 / PI) * dot(ray_out.direction, normal);
	probability = 1 / (2 * PI);
}

void CRTPathtracer::spawnReflectRay(const Intersection& data, const CRTVector& vec_in, CRTRay& ray_out, CRTVector& color_out, float& probability) const
{
	const CRTMaterial& material = scene->getMaterial(data.materialIndex);
	CRTVector normal = data.faceNormal;
	if (material.smoothShading) normal = data.smoothNormal;

	ray_out.origin = data.hitPoint + normal * REFLECTION_BIAS;
	ray_out.direction = reflect(vec_in, normal);
	color_out = scene->getGeometryObject(data.hitObjectIndex).sampleMaterial(material, data) * 1.0f; // written * 1.0f to highlight that the probability could be anywhere up to infinity
	probability = 1.0f;
}

void CRTPathtracer::spawnRefractRay(const Intersection& data, const CRTVector& vec_in, CRTRay& ray_out, CRTVector& color_out, float& probability) const
{
	const CRTMaterial& material = scene->getMaterial(data.materialIndex);
	CRTVector normal = material.smoothShading ? data.smoothNormal : data.faceNormal;

	float n1 = 1.0f; // Index of refraction of the air
	float n2 = material.ior;

	float dotPr = dot(vec_in, normal);
	if (dotPr > 0) { // Ray is leaving the refractive object
		normal *= -1;
		dotPr *= -1;
		std::swap(n1, n2);
	}

	float cosIncomming = -dotPr;
	float sinIncomming = 1.0f - cosIncomming * cosIncomming;

	if (sinIncomming > ((n2 * n2) / (n1 * n1))) { // total internal reflection
		spawnReflectRay(data, vec_in, ray_out, color_out, probability);
		return;
	}

	float sinOutcomming = (n1 / n2) * sqrt(std::max(0.0f, 1.0f - cosIncomming * cosIncomming));
	float cosOutcomming = sqrt(std::max(0.0f, 1.0f - sinOutcomming * sinOutcomming));

	CRTVector A = cosOutcomming * (-normal);
	CRTVector C = vec_in + (cosIncomming * normal);
	C.normalize();
	CRTVector refractedDirection = A + (C * sinOutcomming);
	refractedDirection.normalize();

	// Improved Fresnel calculation using Schlick's approximation
	float R0 = pow((n1 - n2) / (n1 + n2), 2);
	float fresnel = R0 + (1 - R0) * pow(1.0f - cosIncomming, 5);

	if (randFloat() < fresnel) {
		// the fresnel factors how much of the light is in reflection
		ray_out.origin = data.hitPoint + normal * REFLECTION_BIAS;
		ray_out.direction = reflect(vec_in, normal);
		probability = fresnel;
	}
	else {
		// 1 - fresnel determines how much of the light is refracted
		ray_out.origin = data.hitPoint - normal * REFRACTION_BIAS;
		ray_out.direction = refractedDirection;
		probability = 1 - fresnel;
	}
	color_out = scene->getGeometryObject(data.hitObjectIndex).sampleMaterial(material, data);
}

std::vector<PathVertex> CRTPathtracer::getLigthPath(const CRTLight& light) const
{
	std::vector<PathVertex> lightPath;
	PathVertex vert;
	// first vertex of path is the light source
	vert.color = light.getIntensity() * CRTVector(1, 1, 1);
	vert.point = light.getPosition();
	lightPath.push_back(vert);
	int depth = 0;
	CRTVector randomDir = randomSphereSample();
	CRTRay currRay{ light.getPosition(), randomDir, RayType::LIGHT, 0 };
	while (depth < LIGHT_PATH_LENGHT) {
		Intersection ix = rayTraceAccelerated(currRay);
		if (ix.hitObjectIndex == INVALID_IND) return lightPath;
		vert.w_i = currRay.direction;
		vert.point = ix.hitPoint;
		currRay.depth++;

		CRTVector color;
		float probability;
		spawnRay(ix, currRay.direction, currRay, color, probability);
		vert.w_o = currRay.direction;
		vert.color *= color;
		if (probability <= 0) return lightPath;
		if (scene->getMaterial(ix.materialIndex).type == CRTMaterialType::DIFFUSE)
		{
			vert.normal = scene->getMaterial(ix.materialIndex).smoothShading ? ix.smoothNormal : ix.faceNormal;
			lightPath.push_back(vert);
		}
		depth++;
	}

	return lightPath;
}

std::vector<PathVertex> CRTPathtracer::getCameraPath(const CRTRay& ray) const
{
	std::vector<PathVertex> path;
	CRTRay currRay = ray;
	int depth = 0;
	PathVertex vert;
	vert.color = CRTVector(1, 1, 1);
	while (depth < CAM_PATH_LENGHT) {
		Intersection ix = rayTraceAccelerated(currRay);
		if (ix.hitObjectIndex == INVALID_IND) return path;
		vert.w_i = currRay.direction;
		vert.point = ix.hitPoint;
		currRay.depth++;

		CRTVector color;
		float probability;
		spawnRay(ix, currRay.direction, currRay, color, probability);
		vert.w_o = currRay.direction;
		vert.color = color;
		if (probability <= 0) return path;
		if (scene->getMaterial(ix.materialIndex).type == CRTMaterialType::DIFFUSE)
		{
			vert.normal = scene->getMaterial(ix.materialIndex).smoothShading ? ix.smoothNormal : ix.faceNormal;
			path.push_back(vert); // so we only check the diffuse materials in the path
		}
		depth++;
	}
	return path;
}

bool CRTPathtracer::connected(const CRTVector& a, const CRTVector& b) const
{
	CRTVector dir = (a - b);
	float len = dir.length();
	dir.normalize();
	CRTRay ray{ b, dir, RayType::SHADOW, 0 };
	return rayTraceAccelerated(ray, len).triangleIndex != NO_HIT_INDEX;
}

CRTVector CRTPathtracer::directIllumination(const PathVertex& data, const CRTLight& light) const
{
	// slightly modified version of the shader for diffuse materials
	CRTVector finalColor{ 0.0, 0.0, 0.0 };

	CRTVector lightDirection = light.getPosition() - data.point;
	float sphereRadius = lightDirection.length();
	float sphereArea = 4 * PI * sphereRadius * sphereRadius;
	lightDirection.normalize();
	float cosLaw = std::max(0.0f, dot(lightDirection, data.normal));

	if (connected(data.point, light.getPosition())) {
		float lightIntensity = light.getIntensity() / sphereArea;
		finalColor += data.color * lightIntensity * cosLaw / PI;
	}
	return finalColor.clamp(0, 1);
}

CRTVector CRTPathtracer::computePathColor(const std::vector<PathVertex>& cameraPath, int cameraNode, const std::vector<PathVertex>& lightPath, int lightNode) const
{
	if (!connected(cameraPath[cameraNode].point, lightPath[lightNode].point))
		return CRTVector(0, 0, 0);
	// connect the paths
	CRTVector connection = (cameraPath[cameraNode].point - lightPath[lightNode].point);
	float distance = connection.length();
	connection.normalize();

	CRTVector C = cameraPath[cameraNode].color * lightPath[lightNode].color;
	float G = dot(cameraPath[cameraNode].normal, connection) * dot(lightPath[lightNode].normal, connection) / (distance * distance);
	
	if (G < 0) G = -G;

	return C * G;
}


CRTVector CRTPathtracer::computeColor(const CRTRay& cameraRay, CRTImage& image) const
{
	std::vector<PathVertex> cameraPath = getCameraPath(cameraRay);
	CRTVector finalColor(0, 0, 0);
	for (int i = 0; i < cameraPath.size(); i++) {
		for (auto& light : scene->getLights()) {
			std::vector<PathVertex> lightPath = getLigthPath(light);
			//finalColor += directIllumination(cameraPath[i], light);

			for (int j = 1; j < lightPath.size(); j++) {
				//finalColor += computePathColor(cameraPath, i, lightPath, j);
				CRTVector camDirection = (lightPath[j].point - scene->getCamera().getPosition()); // shoot from camera to have backface culling
				float dist = camDirection.length();
				camDirection.normalize();

				CRTRay cameraRay{scene->getCamera().getPosition(), camDirection, RayType::CAMERA, 0 };
				Intersection ix = rayTraceAccelerated(cameraRay, dist);

				if (ix.triangleIndex != NO_HIT_INDEX && ix.hitPoint == lightPath[j].point) {
					cameraRay.type = RayType::SHADOW; // to avoid backface culling with image plane
					auto pixelProjection = scene->getCamera().getRayHitpoint(cameraRay);
					image[pixelProjection.second][pixelProjection.first] += lightPath[j].color;
				}	
			}
		}
	}

	return finalColor;
}
