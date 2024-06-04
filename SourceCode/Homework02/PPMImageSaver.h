#pragma once
#include <fstream>
#include <vector>

struct Color {
	int r;
	int g;
	int b;
};

typedef std::vector<std::vector<Color>> Image;

const unsigned IMAGE_WIDTH = 1920;
const unsigned IMAGE_HEIGHT = 1080;
const unsigned MAX_COLOR_COMPONENT = 255;

const char LINE_SEP = '\n';
const char PIXEL_SEP = '\t';



class PPMImageSaver
{
public:
	static void savePixel(std::ofstream& ppm, const Color& pixel);
	static void saveImage(const char* imgName, const Image& image);
	static void saveImage(std::ofstream& ppm, const Image& image);
};

