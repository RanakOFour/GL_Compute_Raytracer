/**
 * @file Triangle.h
 * @brief Triangle structure for raytracer geometry
 * 
 * This file provides the Triangle struct which stores vertex positions,
 * normals, UV coordinates, and material references. The struct is aligned
 * for efficient GPU upload.
 */

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "GLM/glm.hpp"

/**
 * @struct Triangle
 * @brief GPU-aligned triangle structure for raytracing
 * 
 * Stores the three vertices, the normal, UV coordinates for each vertex,
 * and associated texture and material IDs. Padding is added for 16-byte
 * alignment required by OpenGL SSBOs.
 */
struct Triangle
{
    /** @brief First vertex position */
    glm::vec3 a;
    /** @brief Padding for 16-byte alignment */
    float _padding_a;
    
    /** @brief Second vertex position */
    glm::vec3 b;
    /** @brief Padding for 16-byte alignment */
    float _padding_b;
    
    /** @brief Third vertex position */
    glm::vec3 c;
    /** @brief Padding for 16-byte alignment */
    float _padding_c;
    
    /** @brief Face normal vector */
    glm::vec3 normal;
    /** @brief Padding for 16-byte alignment */
    float _padding_n;

    /** @brief UV coordinate for vertex A */
    glm::vec2 uvA;
    /** @brief UV coordinate for vertex B */
    glm::vec2 uvB;
    /** @brief UV coordinate for vertex C */
    glm::vec2 uvC;
    
    /** @brief Index into texture array (-1 for no texture) */
    int textureId;
    /** @brief Index into material array */
    int materialId;
};

/**
 * @brief Calculate and set the normal vector of a triangle
 * @param _tri Reference to the triangle to update
 * 
 * Calculates the face normal using cross product of edges.
 * Implemented as a free function to avoid issues with GPU SSBO layout.
 */
inline void CalculateNormal(Triangle& _tri)
{
    glm::vec3 edge1 = _tri.c - _tri.a;
    glm::vec3 edge2 = _tri.b - _tri.a;
    _tri.normal = glm::normalize(glm::cross(edge1, edge2));
};

/**
 * @brief Calculate the centroid (center point) of a triangle
 * @param _tri Reference to the triangle
 * @return 3D position at the center of the triangle face
 * 
 * Implemented as a free function to avoid issues with GPU SSBO layout.
 */
inline glm::vec3 CalculateCentroid(Triangle& _tri)
{
    return (_tri.a + _tri.b + _tri.c) * 0.333f;
};

#endif