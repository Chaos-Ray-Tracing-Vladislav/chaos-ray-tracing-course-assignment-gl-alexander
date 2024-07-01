#pragma once
#include "../CRTRenderer.h"

int homework9_0() {
	CRTScene scene("Homework09/scene0.crtscene");
	CRTRenderer renderer(&scene);
	renderer.renderSceneBarycentic("Images/Homework09/scene0.ppm");
	return 0;
}

int homework9_1() {
	CRTScene scene("Homework09/scene1.crtscene");
	CRTRenderer renderer(&scene);
	renderer.renderSceneBarycentic("Images/Homework09/scene1.ppm");
	return 0;
}

int homework9_2() {
	CRTScene scene("Homework09/scene2.crtscene");
	CRTRenderer renderer(&scene);
	renderer.renderScene("Images/Homework09/scene2.ppm");
	return 0;
}

int homework9_3() {
	CRTScene scene("Homework09/scene3.crtscene");
	CRTRenderer renderer(&scene);
	renderer.renderScene("Images/Homework09/scene3.ppm");
	return 0;
}

int homework9_4() {
	CRTScene scene("Homework09/scene4.crtscene");
	CRTRenderer renderer(&scene);
	renderer.renderScene("Images/Homework09/scene4.ppm");
	return 0;
}

int homework9_5() {
	CRTScene scene("Homework09/scene5.crtscene");
	CRTRenderer renderer(&scene);
	renderer.renderScene("Images/Homework09/scene5.ppm");
	return 0;
}

int main()
{
	return homework9_5();
	return 0;
}
