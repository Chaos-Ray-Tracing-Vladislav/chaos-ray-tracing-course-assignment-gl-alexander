#pragma once
#include <iostream>

constexpr float EPSILON = 0.00001;
constexpr float PI = 3.1415;

// generates a random float between 0 and 1
inline float randFloat() {
	return (float)rand() / RAND_MAX;
}

inline int randomInt(int start, int end) {
	return rand() % (end - start) + start;
}

// clams the value between start and end
inline float clampValue(float original, float start, float end) {
	if (original < start) return start;
	if (original > end) return end;
	return original;
}

inline void randomCircleSample(float& x, float& y) {
	do {
		x = randFloat();
		y = randFloat();
	} while (x * x + y * y > 1);
}