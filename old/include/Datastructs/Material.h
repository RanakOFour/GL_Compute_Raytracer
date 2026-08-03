/**
 * @file Material.h
 * @brief Material structure for PBR surface properties
 * 
 * This file provides the Material struct which stores physically-based
 * rendering (PBR) material properties for raytraced surfaces.
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include "GLM/ext.hpp"

/**
 * @struct Material
 * @brief PBR material properties for raytraced surfaces
 * 
 * Contains standard PBR material parameters including albedo color,
 * metallic, roughness, and ambient occlusion values.
 */
struct Material
{
    /** @brief Base color/albedo of the material (RGB) */
    glm::vec3 albedo;

    /** @brief Metallic factor (0 = dielectric, 1 = metal) */
    float metallic;
    
    /** @brief Surface roughness (0 = smooth/mirror, 1 = rough/diffuse) */
    float roughness;
    
    /** @brief Ambient occlusion factor (0 = fully occluded, 1 = no occlusion) */
    float ambientOcclusion;
};

#endif