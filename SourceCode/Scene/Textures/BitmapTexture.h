#pragma once
#include "Texture.h"
#include "../../Utils/CRTColor.h"

class BitmapTexture : public Texture
{
	int width;
	int height;
	int channels;
	uint8_t* image;
public:
	BitmapTexture(const std::string& name, const std::string& imagePath);
	CRTVector sample(float u, float v, const CRTVector& bary) const override;
	~BitmapTexture();
};

