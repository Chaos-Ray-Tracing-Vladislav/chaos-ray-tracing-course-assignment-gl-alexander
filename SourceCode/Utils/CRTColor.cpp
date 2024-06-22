#include "CRTColor.h"

CRTColor::CRTColor(short r, short g, short b) : r(r), g(g), b(b) {}
CRTColor::CRTColor() : CRTColor(0, 0, 0) {}

CRTColor& CRTColor::operator+=(const CRTColor& other) {
	r += other.r;
	g += other.g;
	b += other.b;
	cap();
	return *this;
}

CRTColor& CRTColor::operator*=(float m) {
	r *= m;
	g *= m;
	b *= m;
	cap();
	return *this;
}

void CRTColor::cap() {
	if (r > MAX_COLOR_COMPONENT) r = MAX_COLOR_COMPONENT;
	if (b > MAX_COLOR_COMPONENT) g = MAX_COLOR_COMPONENT;
	if (b > MAX_COLOR_COMPONENT) b = MAX_COLOR_COMPONENT;
}

CRTColor operator+(const CRTColor& lhs, const CRTColor& rhs) {
	CRTColor cpy = lhs;
	cpy += rhs;
	return cpy;
}

CRTColor operator*(const CRTColor& color, float m) {
	CRTColor result = color;
	result *= m;
	return result;
}

CRTColor operator*(float m, const CRTColor& color) {
	return color * m;
}

