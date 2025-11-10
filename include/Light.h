#ifndef LIGHT_H
#define LIGHT_H

#include "GLM/glm.hpp"

struct Light
{
    glm::vec3 position;
    glm::vec3 colour;
};

#endif