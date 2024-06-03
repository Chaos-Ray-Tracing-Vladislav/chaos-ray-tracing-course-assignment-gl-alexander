#include <fstream>
#include <vector>
#include <functional>
#include <random>

static const unsigned IMAGE_WIDTH = 800;
static const unsigned IMAGE_HEIGHT = 800;
static const unsigned MAX_COLOR_COMPONENT = 255;

static const char LINE_SEP = '\n';
static const char PIXEL_SEP = '\t';

struct Color {
    int r;
    int g;
    int b;
};

typedef std::vector<std::vector<Color>> Image;

void savePixel(std::ofstream& ppm, const Color& pixel);
void saveImage(const char* imgName, const Image& image);
void saveImage(std::ofstream& ppm, const Image& image);



void savePixel(std::ofstream& ppm, const Color& pixel) {
    // we save control over what to print before and after each pixel
    ppm << (int)pixel.r << " " << (int)pixel.g << " " << (int)pixel.b;
}

void saveImage(const char* imgName, const Image& image) {
    std::ofstream ppmFileStream(imgName, std::ios::out | std::ios::trunc);
    ppmFileStream << "P3" << LINE_SEP;
    ppmFileStream << IMAGE_WIDTH << " " << IMAGE_HEIGHT << LINE_SEP;
    ppmFileStream << MAX_COLOR_COMPONENT << LINE_SEP;

    saveImage(ppmFileStream, image);

    ppmFileStream.close();
}

void saveImage(std::ofstream& ppm, const Image& image) {
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


    saveImage("crt_subdivisions_square.ppm", image);
}
