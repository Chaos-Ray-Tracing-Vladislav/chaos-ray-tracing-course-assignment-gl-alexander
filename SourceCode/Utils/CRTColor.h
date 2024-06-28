#pragma once
#include <iostream>

const short MAX_COLOR_COMPONENT = 255;

struct CRTColor {
	short r;
	short g;
	short b;
	CRTColor(short r, short g, short b);
	CRTColor();

	CRTColor& operator+=(const CRTColor& other);

	CRTColor& operator*=(float m);
private:
	void cap();
};

CRTColor operator+(const CRTColor& lhs, const CRTColor& rhs);

CRTColor operator*(const CRTColor& color, float m);

CRTColor operator*(float m, const CRTColor& color);

