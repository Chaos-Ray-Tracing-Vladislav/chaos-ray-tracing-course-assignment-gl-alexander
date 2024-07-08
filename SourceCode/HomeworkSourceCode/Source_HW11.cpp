#pragma once
#include "../CRTRenderer.h"
#include "../Scene/CRTSceneFactory.h"

int main()
{
	CRTScene* scene = CRTSceneFactory::factory("scene8.crtscene");
	CRTRenderer renderer(scene);
	renderer.renderScene("../Images/Homework11/scene8.ppm");
	delete scene;
	return 0;
}
