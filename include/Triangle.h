#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "GLM/glm.hpp"

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
};

// Outside of struct so they can be uploaded to opengl without any further padding nonsense;
inline void CalculateNormal(Triangle& _tri)
{
    glm::vec3 edge1 = _tri.c - _tri.a;
    glm::vec3 edge2 = _tri.b - _tri.a;
    _tri.normal = glm::normalize(glm::cross(edge1, edge2));
};

#endif