#ifndef LIGHT_H
#define LIGHT_H

#include "GLM/glm.hpp"

/*
*   Stores CPU side information about point lights
*/
struct Light
{
    glm::vec3 position;
    glm::vec3 colour;
    float intensity;

    // Data for sphere light
    float radius;

    // Data for plane light
    glm::vec3 normal;
    glm::vec3 halfExtents;
};

#endif