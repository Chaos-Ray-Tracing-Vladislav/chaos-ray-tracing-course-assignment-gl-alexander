#include "BitmapTexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

BitmapTexture::BitmapTexture(const std::string& name, const std::string& imagePath) : Texture(name)
{
	image = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);
	if (image == nullptr) {
		throw std::invalid_argument("No such image found");
	}
}

CRTVector BitmapTexture::sample(float u, float v, const CRTVector& bary) const
{
	unsigned pixel_x = u * (float)width;
	unsigned pixel_y = (1.0 - v) * (float)height;
	unsigned bytePerPixel = channels;
	unsigned char* pixelOffset = image + (pixel_x + width * pixel_y) * bytePerPixel;
	float multiValue = 1.0f / MAX_COLOR_COMPONENT;
	float r = (float)pixelOffset[0] * multiValue;
	float g = (float)pixelOffset[1] * multiValue;
	float b = (float)pixelOffset[2] * multiValue;
	return CRTVector(r, g, b);
}

BitmapTexture::~BitmapTexture()
{
	stbi_image_free(image);
	width = height = channels = 0;
	image = nullptr;
}
