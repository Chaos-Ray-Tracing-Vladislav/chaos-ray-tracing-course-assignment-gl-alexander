#pragma once
#include "../Utils/CRTVector.h"

struct CRTImageSettings {
	unsigned width;
	unsigned height;
	unsigned bucketSize;
};

struct CRTSettings
{
	CRTVector bgColor;
	CRTImageSettings imageSettings;
};

