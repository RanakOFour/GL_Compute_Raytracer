#ifndef MATERIAL_H
#define MATERIAL_H

#include "GLM/ext.hpp"

struct Material
{
    glm::vec3 albedo;

    float metallic;
    float roughness;
    float ambientOcclusion;
};

#endif