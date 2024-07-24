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
		path.emplace_back(ix, color, probability); // so we only check the diffuse materials in the path
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
	float ior = material.ior;  // Index of refraction
	CRTVector normal = material.smoothShading ? data.smoothNormal : data.faceNormal;
	CRTVector refracted;
	if (refract(vec_in, normal, refracted, ior)) {
		float n1 = 1.0f, n2 = material.ior;
		float dotPr = dot(vec_in, normal);
		if (dotPr > 0) { // Ray is leaving the refractive object
			normal *= -1;
			dotPr *= -1;
			std::swap(n1, n2);
		}
		float R0 = pow((n1 - n2) / (n1 + n2), 2);
		float fresnel = R0 + (1 - R0) * pow(1.0f + dotPr, 5);
		if (randFloat() <= fresnel) { // using chance to determine the next ray to trace
			ray_out.origin = data.hitPoint + normal * REFLECTION_BIAS;
			ray_out.direction = reflect(vec_in, normal);
		}
		else {
			ray_out.origin = data.hitPoint + normal * REFRACTION_BIAS;
			ray_out.direction = refracted;
		}
		
		color_out = scene->getGeometryObject(data.hitObjectIndex).sampleMaterial(material, data);
		probability = 1.0f;
	}
	else { // then we have only reflection
		spawnReflectRay(data, vec_in, ray_out, color_out, probability);
	}
}

bool CRTPathtracer::connected(const CRTVector& a, const CRTVector& b) const
{
	CRTVector dir = (a - b).normalize();
	CRTRay ray{ b, dir, RayType::SHADOW, 0 };
	return rayTraceAccelerated(ray).triangleIndex != NO_HIT_INDEX;
}


bool CRTPathtracer::refract(const CRTVector& incomming, CRTVector& normal, CRTVector& refracted, float ior) const
{
	float n1 = 1.0f; // Index of refraction of the air
	float n2 = ior;

	float dotPr = dot(incomming, normal);
	if (dotPr > 0) { // Ray is leaving the refractive object
		normal *= -1;
		dotPr *= -1;
		std::swap(n1, n2);
	}

	float cosIncomming = -dotPr;
	float sinIncomming = 1.0f - cosIncomming * cosIncomming;

	if (sinIncomming > ((n2 * n2) / (n1 * n1))) { // total internal reflection
		return false;
	}

	float sinOutcomming = (n1 / n2) * sqrt(std::max(0.0f, 1.0f - cosIncomming * cosIncomming));
	float cosOutcomming = sqrt(std::max(0.0f, 1.0f - sinOutcomming * sinOutcomming));

	CRTVector A = cosOutcomming * (-normal);
	CRTVector C = incomming + (cosIncomming * normal);
	C.normalize();
	refracted = A + (C * sinOutcomming);
	refracted.normalize();
	return true;
}

std::vector<PathVertex> CRTPathtracer::getLigthPath(const CRTLight& light) const
{
	std::vector<PathVertex> lightPath;

	CRTVector randomDir = randomSphereSample();
	CRTRay lightRay{ light.getPosition(), randomDir, RayType::LIGHT, 0 };
	auto path = getPath(lightRay);
	lightPath.insert(lightPath.end(), path.begin(), path.end());

	return lightPath;
}

std::vector<PathVertex> CRTPathtracer::getPath(const CRTRay& ray) const
{
	std::vector<PathVertex> path;
	pathTrace(ray, CRTVector(1, 1, 1), path);
	return path;
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
			for (int x = 0; x < squaresPerSide; x++) {
				for (int y = 0; y < squaresPerSide; y++) {
					squareCenterX = x * side + halfside;
					squareCenterY = y * side + halfside;
					CRTRay ray = scene->getCamera().getRayForSubpixel(rowId + squareCenterY, colId + squareCenterX);
					finalColor += computeColor(ray);
				}
			}
			for (int i = squareCount; i < scene->getSettings().raysPerPixel; i++) {
				// generate the rest using montecarlo
				CRTRay ray = scene->getCamera().getRayForSubpixel(rowId + randFloat(), colId + randFloat());
				finalColor += computeColor(ray);
			}
			output[rowId][colId] = (finalColor * mult).clamp(0, 1);
		}
	}
}


CRTVector CRTPathtracer::directIllumination(const Intersection& data, const CRTLight& light) const
{
	// slightly modified version of the shader for diffuse materials
	CRTVector finalColor{ 0.0, 0.0, 0.0 };
	CRTVector normalVector = data.faceNormal;
	const CRTMaterial& material = scene->getMaterial(data.materialIndex);
	if (material.smoothShading) {
		normalVector = data.smoothNormal;
	}
	CRTVector lightDirection = light.getPosition() - data.hitPoint;
	float sphereRadius = lightDirection.length();
	float sphereArea = 4 * PI * sphereRadius * sphereRadius;
	lightDirection.normalize();
	float cosLaw = std::max(0.0f, dot(lightDirection, normalVector));
	if (connected(data.hitPoint, light.getPosition())) {
		float multValue = light.getIntensity() / sphereArea * cosLaw;
		finalColor += scene->getGeometryObject(data.hitObjectIndex)
			.sampleMaterial(scene->getMaterial(data.materialIndex), data) * multValue / PI;
	}
	return finalColor.clamp(0, 1);
}

CRTVector CRTPathtracer::computePathColor(const std::vector<PathVertex>& cameraPath, int cameraNode, const std::vector<PathVertex>& lightPath, int lightNode) const
{
	if (!connected(cameraPath[cameraNode].intersection.hitPoint, lightPath[lightNode].intersection.hitPoint))
		return CRTVector(0, 0, 0);
	// connect the paths
	CRTVector lightVertNormal = scene->getMaterial(lightPath[lightNode].intersection.materialIndex).smoothShading
		? lightPath[lightNode].intersection.smoothNormal : lightPath[lightNode].intersection.faceNormal;
	CRTVector camVertNormal = scene->getMaterial(cameraPath[cameraNode].intersection.materialIndex).smoothShading
		? cameraPath[cameraNode].intersection.smoothNormal : cameraPath[cameraNode].intersection.faceNormal;

	CRTVector connection = (cameraPath[cameraNode].intersection.hitPoint - lightPath[lightNode].intersection.hitPoint);
	float distance = connection.length();
	connection.normalize();

	CRTVector C(1, 1, 1);
	for (int i = 0; i < cameraNode; i++) {
		C *= cameraPath[i].color;
	}

	float G = dot(camVertNormal, connection) * dot(lightVertNormal, connection) / distance;
	// camera intersection connects to light intersection
	C *= cameraPath[cameraNode].color * lightPath[lightNode].color * G;

	for (int i = lightNode - 1; i >= 0; i--) { // trace the lights backwards since they start from the light
		C *= lightPath[i].color;
	}
	return C;
}


CRTVector CRTPathtracer::computeColor(const CRTRay& cameraRay) const
{
	std::vector<PathVertex> cameraPath;
	pathTrace(cameraRay, CRTVector(1, 1, 1), cameraPath);
	CRTVector finalColor(0, 0, 0);

	for (int i = 0; i < cameraPath.size(); i++) {
		for (auto& light : scene->getLights()) {
			finalColor += directIllumination(cameraPath[i].intersection, light);

			std::vector<PathVertex> lightPath = getLigthPath(light);
			for (int j = 0; j < lightPath.size(); j++) {
				finalColor += computePathColor(cameraPath, i, lightPath, j);
			}
		}
		
	}
	return finalColor;
}
