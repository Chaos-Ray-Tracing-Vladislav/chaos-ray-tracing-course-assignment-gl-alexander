#pragma once
#include "../CRTRenderer.h"

int homework8() {
	CRTScene scene("Homework08/scene3.crtscene");
	CRTRenderer render(&scene);
	render.renderScene("Images/scene3_output.ppm");
	return 0;
}