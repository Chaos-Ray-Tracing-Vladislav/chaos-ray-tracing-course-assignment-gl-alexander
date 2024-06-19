#include "CRTScreen.h"


CRTScreen::CRTScreen(const CRTCamera& camera, unsigned width, unsigned height) : CRTScreen(camera, CRTVector(0, 0, -1), width, height)
{}

CRTScreen::CRTScreen(const CRTCamera& camera, const CRTVector& offset, unsigned width, unsigned height)
	: camera(camera), offset(offset), width(width), height(height) {}

CRTRay CRTScreen::getRayForPixel(unsigned rowId, unsigned colId) const {
	float x_coordinate = static_cast<float>(colId);
	float y_coordinate = static_cast<float>(rowId);
	float z_coordinate = 0;		// relative to the screen, its z is 0

	// Algorithm from https://github.com/Chaos-Ray-Tracing-Vladislav/Homework/blob/main/HomeworkTasks/03%20Rays/CRT%20Homework%2003%20Rays.pdf
	// Convert to NDC space
	x_coordinate /= width;
	y_coordinate /= height;
	
	// Convert NDC coordiantes to Screen space 
	x_coordinate = (2.0 * x_coordinate) - 1.0;
	y_coordinate = 1.0 - (2.0 * y_coordinate);

	// Consider the aspect ratio
	x_coordinate *= width / height;

	// Account for the desired pixel length
	x_coordinate *= PIXEL_LENGTH;
	y_coordinate *= PIXEL_LENGTH;
	
	// Convert to World space coordinates based on screen position
	CRTVector direction(x_coordinate, y_coordinate, z_coordinate);
	direction += offset; // since vec.-matrix multiplication is distributive, so we can add the offset to the pixel direction
						// and rotate it afterwards
	direction = direction * camera.getRotation();

	direction.normalize();
	return CRTRay(camera.getPosition(), direction);
}


std::vector<std::vector<CRTRay>> CRTScreen::generateRays() const {
	std::vector<std::vector<CRTRay>> rayArray;
	rayArray.assign(height, std::vector<CRTRay>(width));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rayArray[i][j] = getRayForPixel(i, j);
		}
	}
	return rayArray;
}

unsigned CRTScreen::getWidth() const {
	return width;
}

unsigned CRTScreen::getHeight() const {
	return height;
}

const CRTCamera& CRTScreen::getCamera() const
{
	return camera;
}
