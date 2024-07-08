#pragma once
#include "../Utils/CRTVector.h"

struct CRTImageSettings {
	unsigned width;
	unsigned height;
};

struct CRTSettings
{
	CRTVector bgColor;
	CRTImageSettings imageSettings;
};

