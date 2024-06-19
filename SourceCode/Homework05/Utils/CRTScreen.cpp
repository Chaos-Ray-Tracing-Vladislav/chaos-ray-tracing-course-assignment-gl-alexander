#include "CRTScreen.h"


CRTScreen::CRTScreen(const CRTVector& position, unsigned width, unsigned height) 
	: position(position), width(width), height(height) {}

CRTRay CRTScreen::getRayForPixel(unsigned rowId, unsigned colId, const CRTVector& cameraPosition) const {
	float x_coordinate = static_cast<float>(colId);
	float y_coordinate = static_cast<float>(rowId);
	float z_coordinate = position.z;

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
	x_coordinate += position.x;
	y_coordinate += position.y;

	CRTVector direction(x_coordinate, y_coordinate, z_coordinate);
	direction.normalize();
	return CRTRay(cameraPosition, direction);
}


std::vector<std::vector<CRTRay>> CRTScreen::generateRays(const CRTVector& cameraPosition) const {
	std::vector<std::vector<CRTRay>> rayArray;
	rayArray.assign(height, std::vector<CRTRay>(width));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rayArray[i][j] = getRayForPixel(i, j, cameraPosition);
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