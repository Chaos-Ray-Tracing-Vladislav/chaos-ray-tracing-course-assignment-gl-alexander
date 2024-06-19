#pragma once
#include <iostream>

const short MAX_COLOR_COMPONENT = 255;

struct CRTColor {
	short r;
	short g;
	short b;
	CRTColor(short r, short g, short b) : r(r), g(g), b(b) {}
	CRTColor() : CRTColor(0, 0, 0) {}
};

