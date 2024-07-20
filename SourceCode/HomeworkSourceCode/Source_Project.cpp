#pragma once
#include "CRTRaytracer.h"
#include "../Scene/CRTAnimation.h"
#include "../Scene/CRTSceneFactory.h"
#include <chrono>

using namespace std::chrono;

CRTImage testAccelerated(CRTRaytracer& renderer) {
	high_resolution_clock::time_point start = high_resolution_clock::now();
	CRTImage result = renderer.renderAccelerated();
	high_resolution_clock::time_point stop = high_resolution_clock::now();
	microseconds duration = duration_cast<microseconds>(stop - start);
	double seconds = duration.count() / 1'000'000.0;
	std::cout << "Rendering time: " << seconds << std::endl;
	return result;
}

void GI_test(void) {
    CRTScene* scene = CRTSceneFactory::factory("Project/scene2.crtscene");
	CRTRaytracer raytracer(scene);

	CRTImageSaver::saveImage("Images/Project/test.ppm", testAccelerated(raytracer));
	delete scene;
}

void GI_animation(void) {
    CRTScene* scene = CRTSceneFactory::factory("Project/scene2.crtscene");
	CRTRaytracer raytracer(scene);
	CRTAnimation animation(scene->getCamera());

	renderer.renderAnimation("Images/Project/Animation/GI_vertigo",
		animation.generateVertigoKeyframes(4, abs(-4.5 - scene->getCamera().getPosition().z), 30));
	delete scene;
}

void dragonAnimation(void) {
    CRTScene* scene = CRTSceneFactory::factory("Homework14/scene1.crtscene");
	CRTRaytracer raytracer(scene);
	CRTAnimation animation(scene->getCamera());

	renderer.renderAnimation("Images/Project/Animation/dragon_orbit", 
		animation.generateVertigoKeyframes(4, abs(-4.5 - scene->getCamera().getPosition().z), 30));
	delete scene;
}

int main()
{
	GI_test();
    GI_animation();
    dragonAnimation();
}
