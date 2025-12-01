#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "GLM/glm.hpp"

/**
 * Stores the three vertexes and normal of a triangle
 * Padding bits added for bit alignment with OpenGL
*/

struct Triangle
{
    glm::vec3 a;
    float _padding_a;
    glm::vec3 b;
    float _padding_b;
    glm::vec3 c;
    float _padding_c;
    glm::vec3 normal;
    float _padding_n;
    glm::vec3 centroid;
    float _padding_cent;
    glm::vec2 uvA;
    glm::vec2 uvB;
    glm::vec2 uvC;
    float _passing_uv[2];
};

/**
 * Calculates normal of given Triangle.
 * Taken outside of Triangle struct to avoid OpenGL schenanigans
 * with uploading to the GPU
*/
inline void CalculateNormal(Triangle& _tri)
{
    glm::vec3 edge1 = _tri.c - _tri.a;
    glm::vec3 edge2 = _tri.b - _tri.a;
    _tri.normal = glm::normalize(glm::cross(edge1, edge2));
};

/**
 * Calculates center point on the face of the given Triangle.
 * Taken outside of Triangle struct to avoid OpenGL schenanigans
 * with uploading to the GPU
*/
inline void CalculateCentroid(Triangle& _tri)
{
    _tri.centroid = (_tri.a + _tri.b + _tri.c) * 0.333f;
};

#endif