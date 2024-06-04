#include "PPMImageSaver.h"


void PPMImageSaver::savePixel(std::ofstream& ppm, const Color& pixel) {
    // we save control over what to print before and after each pixel
    ppm << (int)pixel.r << " " << (int)pixel.g << " " << (int)pixel.b;
}

void PPMImageSaver::saveImage(const char* imgName, const Image& image) {
    std::ofstream ppmFileStream(imgName, std::ios::out | std::ios::trunc);
    ppmFileStream << "P3" << LINE_SEP;
    ppmFileStream << IMAGE_WIDTH << " " << IMAGE_HEIGHT << LINE_SEP;
    ppmFileStream << MAX_COLOR_COMPONENT << LINE_SEP;

    saveImage(ppmFileStream, image);

    ppmFileStream.close();
}

void PPMImageSaver::saveImage(std::ofstream& ppm, const Image& image) {
    for (int rowId = 0; rowId < IMAGE_HEIGHT; rowId++) {
        for (int colId = 0; colId < IMAGE_WIDTH; colId++) {
            savePixel(ppm, image[rowId][colId]);
            if (colId != IMAGE_WIDTH) {
                ppm << PIXEL_SEP;
            }
        }
        if (rowId < IMAGE_HEIGHT) { // so we don't save extra lines after the end of the image
            ppm << LINE_SEP;
        }
    }
}