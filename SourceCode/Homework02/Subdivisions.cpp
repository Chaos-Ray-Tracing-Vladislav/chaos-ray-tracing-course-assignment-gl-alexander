#include <random>
#include "PPMImageSaver.h"

Color generateRandomColor(std::mt19937& rng) {
    std::uniform_int_distribution<int> colorPicker(0, MAX_COLOR_COMPONENT);
    return { colorPicker(rng), colorPicker(rng), colorPicker(rng) };
}

Color pickRandomComponent(const Color& original, std::mt19937& rng) {
    std::uniform_int_distribution<int> componentPicker(0, 2);
    switch (componentPicker(rng)) {
    case 0:
        return { original.r, 0, 0 };
    case 1:
        return { 0, original.b, 0 };
    default:
        return { 0, 0, original.g };
    }
}

void paintSubdivisionWithNoise(Image& image, int subDivX, int subDivY, unsigned horizontalPixels, 
    unsigned verticalPixels, const Color& color, std::mt19937& rng) {
    int startPixelY = subDivY * verticalPixels;
    int startPixelX = subDivX * horizontalPixels;
    for (int rowId = startPixelY; rowId < startPixelY + verticalPixels; rowId++) {
        for (int colId = startPixelX; colId < startPixelX + horizontalPixels; colId++) {
            image[rowId][colId] = pickRandomComponent(color, rng);
        }
    }
}

void createSubdivisionsWithNoise(Image& image, unsigned horizontalCount, unsigned verticalCount) {
    unsigned pixelsPerRectangleX = IMAGE_WIDTH / horizontalCount;
    unsigned pixelsPerRectangleY = IMAGE_HEIGHT / verticalCount;

    std::random_device device;
    std::mt19937 rng(device());

    for (int i = 0; i < horizontalCount; i++) {
        for (int j = 0; j < verticalCount; j++) {
            // we want every subdivision to have one random color assigned to it
            Color subdivColor = generateRandomColor(rng);
            paintSubdivisionWithNoise(image, i, j, pixelsPerRectangleX, pixelsPerRectangleY, subdivColor, rng);
        }
    }
}

int main()
{
    Color background{ 50, 50, 50 };

    Image image;
    image.assign(IMAGE_HEIGHT, std::vector<Color>(IMAGE_WIDTH, background)); // assigns all the memory in one call to avoid resizes
    
    unsigned horizontalCount = 4;
    unsigned verticalCount = 4;
    createSubdivisionsWithNoise(image, horizontalCount, verticalCount);


    PPMImageSaver::saveImage("crt_subdivisions_square.ppm", image);
}
