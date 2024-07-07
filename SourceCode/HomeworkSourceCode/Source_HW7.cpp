#include <iostream>
#include "../CRTRenderer.h"

int main()
{
	CRTScene scene("scene2.crtscene");
	CRTRenderer renderer(&scene);
	renderer.renderScene("scene2_output.ppm");
}
