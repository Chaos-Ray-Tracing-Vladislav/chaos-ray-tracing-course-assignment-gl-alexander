#pragma once
#include "../Utils/CRTVector.h"

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;
constexpr int BUCKET_SIZE = 24;

struct CRTImageSettings {
	unsigned width = WIDTH;
	unsigned height = HEIGHT;
	unsigned bucketSize = BUCKET_SIZE;
};

struct CRTSettings
{
	CRTVector bgColor;
	CRTImageSettings imageSettings;

	bool reflections = true;
	bool refractions = true;

	bool globalIllumination = true;

	bool FXAA = false;

	bool renderedAA = true;
	int raysPerPixel = 16;

	bool dof = false;
	bool autoFocus = true;
	float focalPlaneDist = -1;
	int dofSamples = 32;
	float fNum = 2.0;

	bool stereoscopy = false;
	float eyeDistance = 0.2f; 
	CRTVector leftEyeColor{ 1, 0, 0 };
	CRTVector rightEyeColor{ 0, 0, 1 };
};

