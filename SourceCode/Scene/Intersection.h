#pragma once
#include "../Utils/CRTVector.h"

const int NO_HIT_INDEX = -1;

struct Intersection {
	CRTVector hitPoint;
	CRTVector faceNormal;
	CRTVector smoothNormal;
	CRTVector barycentricCoordinates;
	int materialIndex;
	int hitObjectIndex = NO_HIT_INDEX;
	int triangleIndex = NO_HIT_INDEX;
	float t;
};