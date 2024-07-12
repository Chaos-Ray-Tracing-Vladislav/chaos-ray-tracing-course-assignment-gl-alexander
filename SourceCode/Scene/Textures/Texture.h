#pragma once
#include "EdgeTexture.h"
#include "CheckerTexture.h"
#include "BitmapTexture.h"
#include "ConstantTexture.h"
#include <variant>
#include <iostream>

using Texture = std::variant<ConstantTexture, EdgeTexture, CheckerTexture, BitmapTexture>;
