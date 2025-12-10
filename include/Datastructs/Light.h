#ifndef LIGHT_H
#define LIGHT_H

#include "GLM/glm.hpp"

/*
*   Stores CPU side information about point lights
*/
struct Light
{
    glm::vec3 points[4];
    glm::vec3 colour;
    float intensity;
    float radius;
};

#endif