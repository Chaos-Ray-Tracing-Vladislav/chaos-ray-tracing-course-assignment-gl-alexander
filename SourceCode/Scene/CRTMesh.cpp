#include "CRTMesh.h"

CRTMesh::CRTMesh(const std::vector<CRTVector>& vertices, const std::vector<int>& triangleVertIndices) : 
    vertices(vertices), triangleVertIndices(triangleVertIndices)
{}

std::tuple<bool, CRTVector, CRTTriangle> CRTMesh::intersectsRay(const CRTRay& ray) const
{
    float closestHitDitance = FLT_MAX;
    CRTVector hitPoint;
    CRTTriangle hitTriangle;
    bool isHit = false;
    for (int i = 0; i < triangleVertIndices.size(); i += VERTICES) {
        CRTTriangle triangle(vertices[triangleVertIndices[i + 0]],
            vertices[triangleVertIndices[i + 1]],
            vertices[triangleVertIndices[i + 2]]);
        std::pair<bool, CRTVector> hit = triangle.intersectsRay(ray);
        if (hit.first) {
            isHit = true;
            float distance = triangle.distanceToPoint(ray.getOrigin());
            if (distance < closestHitDitance) {
                closestHitDitance = distance;
                hitPoint = hit.second;
                hitTriangle = triangle;
            }
        }
    }
    return std::make_tuple(isHit, hitPoint, hitTriangle);
}

bool CRTMesh::intersectsShadowRay(const CRTRay& ray) const
{
    for (int i = 0; i < triangleVertIndices.size(); i += VERTICES) {
        CRTTriangle triangle(vertices[triangleVertIndices[i + 0]],
            vertices[triangleVertIndices[i + 1]],
            vertices[triangleVertIndices[i + 2]]);
        if (triangle.intersectsShadowRay(ray)) {
            return true;
        }
    }
    return false;
}
