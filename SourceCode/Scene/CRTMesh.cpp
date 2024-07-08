#include "CRTMesh.h"

void CRTMesh::calculateFaceNormals() {
    for (int i = 0; i < triangleVertIndices.size(); i += VERTICES) {
        CRTVector v0 = vertices[triangleVertIndices[i + 0]];
        CRTVector v1 = vertices[triangleVertIndices[i + 1]];
        CRTVector v2 = vertices[triangleVertIndices[i + 2]];

        //store them while traversing the face normals
        vertexTriangleParticipation[triangleVertIndices[i + 0]].push_back(i / VERTICES);
        vertexTriangleParticipation[triangleVertIndices[i + 1]].push_back(i / VERTICES);
        vertexTriangleParticipation[triangleVertIndices[i + 2]].push_back(i / VERTICES);
        CRTVector V1 = v1 - v0;
        CRTVector V2 = v2 - v0;

        CRTVector normal = cross(V1, V2); // we set it to the counterclockwise cross-product
        normal.normalize();
        faceNormals[i / VERTICES] = std::move(normal);
    }
}

void CRTMesh::calculateVertexNormals() {
    for (int i = 0; i < vertices.size(); i++) {
        CRTVector vNormal;
        for (int triangleIndex : vertexTriangleParticipation[i]) {
            vNormal += faceNormals[triangleIndex];
        }
        vNormal *= (1.0f / vertexTriangleParticipation[i].size()); // since each face normal has len 1 we only need to scale it down by the number of vectors we've added
        vertexNormals[i] = std::move(vNormal);
    }
}


CRTMesh::CRTMesh(const std::vector<CRTVector>& vertices, const std::vector<CRTVector>& uvs, const std::vector<int>& triangleVertIndices, int materialIndex) :
    vertices(vertices), triangleVertIndices(triangleVertIndices), materialIndex(materialIndex), uvs(uvs)
{
    faceNormals.resize(triangleVertIndices.size() / VERTICES);
    vertexNormals.resize(vertices.size());
    calculateFaceNormals();
    calculateVertexNormals();
}

CRTVector CRTMesh::calculateSmoothNormal(int triangleIndex, const CRTVector& barycentic, const CRTVector& point) const {
    CRTVector v0_VertexNormal = vertexNormals[triangleVertIndices[triangleIndex + 0]];
    CRTVector v1_VertexNormal = vertexNormals[triangleVertIndices[triangleIndex + 1]];
    CRTVector v2_VertexNormal = vertexNormals[triangleVertIndices[triangleIndex + 2]];

    // calculate the normal based on the barycentic coordinates {u, v, w} using the formula
    return v0_VertexNormal * barycentic.z
        + v1_VertexNormal * barycentic.x
        + v2_VertexNormal * barycentic.y;
}

Intersection CRTMesh::intersectsRay(const CRTRay& ray) const
{
    Intersection intersection;

    float closestHitDitance = FLT_MAX;
    Intersection triangle_intersection;
    for (int i = 0; i < triangleVertIndices.size(); i += VERTICES) {
        CRTTriangle triangle(vertices[triangleVertIndices[i + 0]],
            vertices[triangleVertIndices[i + 1]],
            vertices[triangleVertIndices[i + 2]], 
            faceNormals[i / VERTICES]);
        triangle_intersection = std::move(triangle.intersectsRay(ray));
        if (triangle_intersection.triangleIndex != NO_HIT_INDEX) {
            float distance = (triangle_intersection.hitPoint - ray.origin).length();
            if (distance < closestHitDitance) {
                closestHitDitance = distance;
                intersection = std::move(triangle_intersection); // this copies all the data we've already calculated
                intersection.triangleIndex = i;
            }
        }
    }
    if (intersection.triangleIndex == NO_HIT_INDEX) {
        intersection.hitObjectIndex = NO_HIT_INDEX;
        return intersection;
    }
    intersection.smoothNormal = calculateSmoothNormal(intersection.triangleIndex, intersection.barycentricCoordinates, intersection.hitPoint);
    intersection.materialIndex = materialIndex;
    intersection.hitObjectIndex = 0; // so we mark the hit;
    return intersection;
}

CRTVector CRTMesh::getUV(const Intersection& data) const
{
    unsigned v0_index = triangleVertIndices[data.triangleIndex + 0];
    unsigned v1_index = triangleVertIndices[data.triangleIndex + 1];
    unsigned v2_index = triangleVertIndices[data.triangleIndex + 2];
    
    return uvs[v1_index] * data.barycentricCoordinates.x
        + uvs[v2_index] * data.barycentricCoordinates.y
        + uvs[v0_index] * data.barycentricCoordinates.z;
}


int CRTMesh::getMaterialIndex() const {
    return materialIndex;
}