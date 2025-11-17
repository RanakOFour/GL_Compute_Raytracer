#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "GLM/glm.hpp"

struct Triangle
{
    glm::vec3 a;
    int ownerIndex;
    glm::vec3 b;
    float _padding_b;
    glm::vec3 c;
    float _padding_c;
    glm::vec3 normal;
    float _padding_n;
    glm::vec3 centroid;
    float padding_cent;
};

// Outside of struct so they can be uploaded to opengl without any further padding nonsense;
inline void CalculateNormal(Triangle& _tri)
{
    glm::vec3 edge1 = _tri.c - _tri.a;
    glm::vec3 edge2 = _tri.b - _tri.a;
    _tri.normal = glm::normalize(glm::cross(edge1, edge2));
};

inline void CalculateCentroid(Triangle& _tri)
{
    _tri.centroid = (_tri.a + _tri.b + _tri.c) * 0.3333f;
}

#endif