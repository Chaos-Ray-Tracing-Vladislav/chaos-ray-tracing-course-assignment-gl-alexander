#include <iostream>
#include "CRTScreen.h"
#include "CRTImageSaver.h"

CRTColor convertDirectionToColor(const CRTVector& direction) {
    // direction components are floats in the range [-1.0, 1.0], because it's been normalized

    // we convert them to be in the range [0.0, 1.0]
    float normal_x = (direction.x + 1.0) / 2.0;
    float normal_y = (direction.y + 1.0) / 2.0;
    float normal_z = (direction.z + 1.0) / 2.0;

    short colorR = normal_x * MAX_COLOR_COMPONENT;
    short colorG = normal_y * MAX_COLOR_COMPONENT;
    short colorB = normal_z * MAX_COLOR_COMPONENT;

    return { colorR, colorG, colorB };
}

void generateImageByRays(CRTImage& image, std::vector<std::vector<CRTRay>> pixelRays) {
    unsigned imageHeight = image.size();
    unsigned imageWidth = image[0].size();

    for (int rowId = 0; rowId < imageHeight; rowId++) {
        for (int colId = 0; colId < imageWidth; colId++) {
            image[rowId][colId] = convertDirectionToColor(pixelRays[rowId][colId].getDirection());
        }
    }
}

int main()
{
    CRTVector cameraPosition(0.0, 0.0, 0.0);
    CRTVector screenPosition(0.0, 0.0, -1.0);

    CRTScreen screen(screenPosition, 1000, 500);
    std::vector<std::vector<CRTRay>> pixelRays = screen.generateRays(cameraPosition);


    CRTImage image(screen.getHeight(), std::vector<CRTColor>(screen.getWidth()));
    generateImageByRays(image, pixelRays);
    CRTImageSaver::saveImage("raydirections.ppm", image);
}
