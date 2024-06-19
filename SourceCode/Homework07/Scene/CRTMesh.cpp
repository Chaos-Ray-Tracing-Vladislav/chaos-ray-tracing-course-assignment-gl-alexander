#include "CRTMesh.h"

CRTMesh::CRTMesh(const std::vector<CRTVector>& vertices, const std::vector<int>& triangleVertIndices) : 
    vertices(vertices), triangleVertIndices(triangleVertIndices)
{}

std::vector<CRTTriangle> CRTMesh::getTriangles() const
{
    std::vector<CRTTriangle> result;
    size_t trianglesCount = triangleVertIndices.size() / VERTICES;
    for (int i = 0; i < trianglesCount; i++) {
        size_t start = i * VERTICES;
        result.push_back(CRTTriangle(vertices[triangleVertIndices[start + 0]],
            vertices[triangleVertIndices[start + 1]],
            vertices[triangleVertIndices[start + 2]]
        ));
    }
    return result;
}
