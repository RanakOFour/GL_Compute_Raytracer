#ifndef MATERIAL_H
#define MATERIAL_H

#include "GLM/ext.hpp"

struct Material
{
    glm::vec3 albedo;
    float _padding_a;

    float metallic;
    float roughness;
    float ambientOcclusion;
    float _padding_b;
};

#endif