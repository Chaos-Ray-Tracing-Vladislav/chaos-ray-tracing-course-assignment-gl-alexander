#pragma once
#include "../CRTRenderer.h"
#include "../Scene/CRTSceneFactory.h"
#include <chrono>

using namespace std::chrono;

CRTImage testSingleThreaded(CRTRenderer& renderer) {
	high_resolution_clock::time_point start = high_resolution_clock::now();
	CRTImage result = renderer.renderSinglethreaded();
	high_resolution_clock::time_point stop = high_resolution_clock::now();
	microseconds duration = duration_cast<microseconds>(stop - start);
	double seconds = duration.count() / 1'000'000.0;
	std::cout << "Rendering time: " << seconds << std::endl;
	return result;
	// Benchmark: 340.069 s
}

CRTImage testByRegions(CRTRenderer& renderer) {
	high_resolution_clock::time_point start = high_resolution_clock::now();
	CRTImage result = renderer.renderByRegions();
	high_resolution_clock::time_point stop = high_resolution_clock::now();
	microseconds duration = duration_cast<microseconds>(stop - start);
	double seconds = duration.count() / 1'000'000.0;
	std::cout << "Rendering time: " << seconds << std::endl;
	return result;
	// Benchmark: 112.008 s
}

CRTImage testByBuckets(CRTRenderer& renderer) {
	high_resolution_clock::time_point start = high_resolution_clock::now();
	CRTImage result = renderer.renderByBuckets();
	high_resolution_clock::time_point stop = high_resolution_clock::now();
	microseconds duration = duration_cast<microseconds>(stop - start);
	double seconds = duration.count() / 1'000'000.0;
	std::cout << "Rendering time: " << seconds << std::endl;
	return result;
	// Benchmark: 62.0141 s
}

CRTImage testAABB(CRTRenderer& renderer) {
	high_resolution_clock::time_point start = high_resolution_clock::now();
	CRTImage result = renderer.renderWithAABB();
	high_resolution_clock::time_point stop = high_resolution_clock::now();
	microseconds duration = duration_cast<microseconds>(stop - start);
	double seconds = duration.count() / 1'000'000.0;
	std::cout << "Rendering time: " << seconds << std::endl;
	return result;
	// Benchmark: 14.678 s
}

int main()
{
	CRTScene* scene = CRTSceneFactory::factory("Homework13/scene0.crtscene");
	CRTRenderer renderer(scene);
	
	CRTImageSaver::saveImage("Images/Homework13/scene0.ppm", testAABB(renderer));

	delete scene;
	return 0;
}
