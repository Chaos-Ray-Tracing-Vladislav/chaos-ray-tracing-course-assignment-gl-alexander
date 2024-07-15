#pragma once
#include "../CRTRenderer.h"
#include "../Scene/CRTSceneFactory.h"
#include <chrono>

using namespace std::chrono;	

CRTImage testAccelerated(CRTRenderer& renderer) {
	high_resolution_clock::time_point start = high_resolution_clock::now();
	CRTImage result = renderer.renderAccelerated();
	high_resolution_clock::time_point stop = high_resolution_clock::now();
	microseconds duration = duration_cast<microseconds>(stop - start);
	double seconds = duration.count() / 1'000'000.0;
	std::cout << "Rendering time: " << seconds << std::endl;
	return result;
}

int main()
{
	CRTScene* scene = CRTSceneFactory::factory("Homework14/scene1.crtscene");
	CRTRenderer renderer(scene);
	
	CRTImageSaver::saveImage("Images/Homework14/scene1.ppm", testAccelerated(renderer));

	delete scene;
	return 0;
}
