#pragma once
#include <string>
#include "../../Utils/CRTVector.h"

struct Texture {
    std::string name;

    Texture(const std::string& name) : name(name) {}

    virtual CRTVector sample(float u, float v, const CRTVector& bary) const = 0;
    virtual ~Texture() = default;
};
