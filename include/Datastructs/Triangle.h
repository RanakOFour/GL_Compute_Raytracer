#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "GLM/glm.hpp"

/**
 * Stores the three vertexes, the normal, the uv coordinates associated
 * with each index, and the associated texture and material ids
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
    
    int textureId;
    int materialId;
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

#endif