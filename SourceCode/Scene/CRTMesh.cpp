#include "CRTMesh.h"

void CRTMesh::calculateFaceNormals() {
    for (int i = 0; i < triangleVertIndices.size(); i += VERTICES) {
        CRTVector v0 = vertices[triangleVertIndices[i + 0]];
        CRTVector v1 = vertices[triangleVertIndices[i + 1]];
        CRTVector v2 = vertices[triangleVertIndices[i + 2]];
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
        int triangleParticipations = 0;
        for (int j = 0; j < triangleVertIndices.size(); j++) {
            if (triangleVertIndices[j] == i) { // then a triangle uses the i-th vertex, we add its face normal to our calculation
                int triangleIndex = j / VERTICES; // vertex indices 0 - 2 are from triangle 0, 3 - 5 from triangle 1 and so on
                vNormal += faceNormals[triangleIndex];
                triangleParticipations++; 
            }
        }
        vNormal *= (1.0f / triangleParticipations); // since each face normal has len 1 we only need to scale it down by the number of vectors we've added
        vertexNormals[i] = std::move(vNormal);
    }
}

CRTMesh::CRTMesh(const std::vector<CRTVector>& vertices, const std::vector<int>& triangleVertIndices) : CRTMesh(vertices, triangleVertIndices, DEFAULT_MATERIAL)
{
}

CRTMesh::CRTMesh(const std::vector<CRTVector>& vertices, const std::vector<int>& triangleVertIndices, const CRTMaterial& material) :
    vertices(vertices), triangleVertIndices(triangleVertIndices), material(material)
{
    faceNormals.resize(triangleVertIndices.size() / VERTICES);
    vertexNormals.resize(vertices.size());
    calculateFaceNormals();
    calculateVertexNormals();
}

CRTVector CRTMesh::calculateSmoothNormal(int triangleIndex, const CRTVector& point) const {
    CRTTriangle triangle(vertices[triangleVertIndices[triangleIndex + 0]],
        vertices[triangleVertIndices[triangleIndex + 1]],
        vertices[triangleVertIndices[triangleIndex + 2]],
        faceNormals[triangleIndex / VERTICES]);
    CRTVector barycenticCoordinates = triangle.getBarycenticCoordinates(point);
    CRTVector v0_VertexNormal = vertexNormals[triangleVertIndices[triangleIndex + 0]];
    CRTVector v1_VertexNormal = vertexNormals[triangleVertIndices[triangleIndex + 1]];
    CRTVector v2_VertexNormal = vertexNormals[triangleVertIndices[triangleIndex + 2]];

    // calculate the normal based on the barycentic coordinates {u, v, w} using the formula
    return v0_VertexNormal * barycenticCoordinates.z
        + v1_VertexNormal * barycenticCoordinates.x
        + v2_VertexNormal * barycenticCoordinates.y;
}

// tuple represents: if the mesh is hit, the hitpoint and the normal to the hitpoint
std::tuple<bool, CRTVector, CRTVector> CRTMesh::intersectsRay(const CRTRay& ray) const
{
    float closestHitDitance = FLT_MAX;
    CRTVector hitPoint;
    int hitIndex = -1;
    for (int i = 0; i < triangleVertIndices.size(); i += VERTICES) {
        CRTTriangle triangle(vertices[triangleVertIndices[i + 0]],
            vertices[triangleVertIndices[i + 1]],
            vertices[triangleVertIndices[i + 2]], 
            faceNormals[i / VERTICES]);
        std::pair<bool, CRTVector> hit = triangle.intersectsRay(ray);
        if (hit.first) {
            float distance = (hit.second - ray.getOrigin()).length();
            if (distance < closestHitDitance) {
                hitIndex = i; // the index of the triangle it's hit
                closestHitDitance = distance;
                hitPoint = hit.second;
            }
        }
    }
    if (hitIndex == -1) {
        return std::make_tuple(false, CRTVector(), CRTVector());
    }
    
    if (this->material.smoothShading) {
        return std::make_tuple(true, hitPoint, calculateSmoothNormal(hitIndex, hitPoint));
    }
    else { // here the normal vector is just the triangle face
        return std::make_tuple(true, hitPoint, faceNormals[hitIndex / VERTICES]);
    }
}

// for illustration purposes
std::tuple<bool, CRTVector, CRTVector, CRTVector> CRTMesh::intersectsRayBarycentic(const CRTRay& ray) const
{
    float closestHitDitance = FLT_MAX;
    CRTVector hitPoint;
    int hitIndex = -1;
    for (int i = 0; i < triangleVertIndices.size(); i += VERTICES) {
        CRTTriangle triangle(vertices[triangleVertIndices[i + 0]],
            vertices[triangleVertIndices[i + 1]],
            vertices[triangleVertIndices[i + 2]],
            faceNormals[i / VERTICES]);
        std::pair<bool, CRTVector> hit = triangle.intersectsRay(ray);
        if (hit.first) {
            float distance = triangle.distanceToPoint(ray.getOrigin());
            if (distance < closestHitDitance) {
                hitIndex = i; // the index of the triangle it's hit
                closestHitDitance = distance;
                hitPoint = hit.second;
            }
        }
    }
    if (hitIndex == -1) {
        return std::make_tuple(false, CRTVector(), CRTVector(), CRTVector());
    }
    CRTTriangle triangle(vertices[triangleVertIndices[hitIndex + 0]],
        vertices[triangleVertIndices[hitIndex + 1]],
        vertices[triangleVertIndices[hitIndex + 2]],
        faceNormals[hitIndex / VERTICES]);
    CRTVector barycenticCoordinates = triangle.getBarycenticCoordinates(hitPoint);

    return std::make_tuple(true, hitPoint, triangle.getNormal(), triangle.getBarycenticCoordinates(hitPoint));
}

bool CRTMesh::intersectsShadowRay(const CRTRay& ray) const
{
    for (int i = 0; i < triangleVertIndices.size(); i += VERTICES) {
        CRTTriangle triangle(vertices[triangleVertIndices[i + 0]],
            vertices[triangleVertIndices[i + 1]],
            vertices[triangleVertIndices[i + 2]], 
            faceNormals[i / VERTICES]);
        if (triangle.intersectsShadowRay(ray)) {
            return true;
        }
    }
    return false;
}

CRTMaterial CRTMesh::getMaterial() const
{
    return material;
}
