#pragma once
#include "../Utils/CRTColor.h"

struct CRTImageSettings {
	unsigned width;
	unsigned height;
};

struct CRTSettings
{
	CRTColor bgColor;
	CRTImageSettings imageSettings;
};

