#include "CRTBox.h"
#include <iostream>

bool CRTBox::intersects(const CRTRay& ray) const
{
    float divx, divy, divz, tmin, tmax, tymin, tymax, tzmin, tzmax;
    divx = 1 / ray.direction.x;
    if (divx >= 0) {
        tmin = (min.x - ray.origin.x) * divx;
        tmax = (max.x - ray.origin.x) * divx;
    }
    else {
        tmin = (max.x - ray.origin.x) * divx;
        tmax = (min.x - ray.origin.x) * divx;
    }
    divy = 1 / ray.direction.y;
    if (divy >= 0) {
        tymin = (min.y - ray.origin.y) * divy;
        tymax = (max.y - ray.origin.y) * divy;
    }
    else {
        tymin = (max.y - ray.origin.y) * divy;
        tymax = (min.y - ray.origin.y) * divy;
    }
    if ((tmin > tymax) || (tymin > tmax)) {
        return false;
    }
    if (tymin > tmin) {
        tmin = tymin;
    }
    if (tymax < tmax) {
        tmax = tymax;
    }
    divz = 1 / ray.direction.z;
    if (divz >= 0) {
        tzmin = (min.z - ray.origin.z) * divz;
        tzmax = (max.z - ray.origin.z) * divz;
    }
    else {
        tzmin = (max.z - ray.origin.z) * divz;
        tzmax = (min.z - ray.origin.z) * divz;
    }
    if ((tmin > tzmax) || (tzmin > tmax)) {
        return false;
    }
    if (tzmin > tmin) {
        tmin = tzmin;
    }
    if (tzmax < tmax) {
        tmax = tzmax;
    }
    return true;

}
