#pragma once
#include "../CRTRenderer.h"
#include "../Scene/CRTSceneFactory.h"

int main()
{
	CRTScene* scene = CRTSceneFactory::factory("Homework12/scene4.crtscene");
	CRTRenderer renderer(scene);
	renderer.renderScene("Images/Homework12/scene4.ppm");
	delete scene;
	return 0;
}
