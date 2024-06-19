#include <iostream>
#include "CRTRenderer.h"

int main()
{
	CRTScene scene("scene4.crtscene");
	CRTRenderer renderer(&scene);
	renderer.renderScene("scene4_output.ppm");
}
