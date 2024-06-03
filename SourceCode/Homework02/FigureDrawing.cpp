#include <fstream>
#include <vector>
#include <functional>

static const unsigned IMAGE_WIDTH = 1920;
static const unsigned IMAGE_HEIGHT = 1080;
static const unsigned MAX_COLOR_COMPONENT = 255;

static const char LINE_SEP = '\n';
static const char PIXEL_SEP = '\t';

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
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

void createFigure(Image& image, const Color& figureColor, std::function<bool(int, int)> figurePredicate) {
    // An abstract way of saving the information for figures, since a pixel can either be inside or outside of a figure
    // This colors in the pixels that are inside the figure only
    // Additionally it can be made parallel
    for (int rowId = 0; rowId < IMAGE_HEIGHT; rowId++) {
        for (int colId = 0; colId < IMAGE_WIDTH; colId++) {
            if (figurePredicate(colId, rowId)) {
                image[rowId][colId] = figureColor;
            }
        }
    }
}


void createCircle(Image& image, const Color& circleColor, unsigned radius, int centerX, int centerY) {
    auto pixelInCircle = [radius, centerX, centerY](int x, int y) {
        int relativeX = x - centerX;
        int relativeY = y - centerY;
        return (relativeX * relativeX + relativeY * relativeY) <= (radius * radius);
    };

    createFigure(image, circleColor, pixelInCircle);
}

void createCircle(Image& image, const Color& circleColor, unsigned radius) {
    int centerX = IMAGE_WIDTH / 2;
    int centerY = IMAGE_HEIGHT / 2; // default circle center is the center of the image

    createCircle(image, circleColor, radius, centerX, centerY);
}

void createRectangle(Image& image, const Color& rectColor, unsigned width, unsigned height, int centerX, int centerY) {
    auto pixelInRectangle = [width, height, centerX, centerY](int x, int y) {
        int relativeX = x - centerX;
        int relativeY = y - centerY;
        return (std::abs(relativeX) <= width && std::abs(relativeY) <= height);
    };

    createFigure(image, rectColor, pixelInRectangle);
}

int main()
{
    Color background{ 50, 50, 50 };
    Color circleColor{ 10, 200, 10 };
    unsigned radiusInPixels = 200;

    Color squareColor{ 200, 20, 20 };
    unsigned side = 300;

    Image image;
    image.assign(IMAGE_HEIGHT, std::vector<Color>(IMAGE_WIDTH, background)); // assigns all the memory in one call to avoid resizes
    
    createRectangle(image, squareColor, side, side, IMAGE_WIDTH / 2 - 300, IMAGE_HEIGHT / 2);
    createCircle(image, circleColor, radiusInPixels);
    saveImage("crt_figures.ppm", image);
}

