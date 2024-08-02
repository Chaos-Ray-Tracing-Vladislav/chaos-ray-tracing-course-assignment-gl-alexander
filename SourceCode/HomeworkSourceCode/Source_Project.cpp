#pragma once
#include "CRTRaytracer.h"
#include "CRTPathtracer.h"
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
	CRTPathtracer pathtracer(scene);

	CRTImageSaver::saveImage("Images/Project/pt_scene2.ppm", testAccelerated(raytracer));
	delete scene;
}

void GI_animation(void) {
	CRTScene* scene = CRTSceneFactory::factory("Project/scene2.crtscene");
	CRTAnimation vertigoAnimation = CRTAnimation::vertigoAnimation(scene, 4, 6.62, 30);
	CRTRaytracer raytracer(&vertigoAnimation);

	raytracer.renderAnimation("Images/Project/Animation/GI_vertigo");
	delete scene;
}

void dragonAnimation(void) {
	CRTScene* scene = CRTSceneFactory::factory("Homework14/scene1.crtscene");
	CRTVector sceneMiddle = (scene->getAABB().max + scene->getAABB().min) / 2;
	CRTAnimation orbitAnimation = CRTAnimation::orbitAnimation(scene, sceneMiddle, 72, 5, false);
	CRTRaytracer raytracer(&orbitAnimation);

	raytracer.renderAnimation("Images/Project/Animation/dragon_orbit");
	delete scene;
}

int main()
{
	GI_test();
    GI_animation();
    dragonAnimation();
}
