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
    glm::vec2 uvA;
    glm::vec2 uvB;
    glm::vec2 uvC;
    float _pad3[2];
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
inline glm::vec3 CalculateCentroid(Triangle& _tri)
{
    return (_tri.a + _tri.b + _tri.c) * 0.333f;
};

inline void CalculateUVonPoint(Triangle& _tri, glm::vec3 l_point)
{
    glm::vec3 l_f1 = _tri.a - l_point;
    glm::vec3 l_f2 = _tri.b - l_point;
    glm::vec3 l_f3 = _tri.c - l_point;

    float l_invTriArea = 1.0f / glm::length(glm::cross(_tri.a - _tri.b, _tri.a - _tri.c));

    float l_triArea1 = length(cross(l_f2, l_f3)) * l_invTriArea;
    float l_triArea2 = length(cross(l_f3, l_f1)) * l_invTriArea;
    float l_triArea3 = length(cross(l_f1, l_f2)) * l_invTriArea;

    glm::vec2 uv = _tri.uvA * l_triArea1 + _tri.uvB * l_triArea2 + _tri.uvC * l_triArea3;
    printf("UV for Point: (%f, %f)\n", uv.x, uv.y);
};

#endif