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
};

#endif