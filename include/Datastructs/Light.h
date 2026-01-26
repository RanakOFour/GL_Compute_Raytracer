/**
 * @file Light.h
 * @brief Light structure for CPU-side light representation
 * 
 * This file provides the Light struct which stores light properties
 * for point lights and area lights in the raytracer scene.
 */

#ifndef LIGHT_H
#define LIGHT_H

#include "GLM/glm.hpp"

/**
 * @struct Light
 * @brief Stores CPU-side information about scene lights
 * 
 * Contains data for both point lights (position, color, intensity, radius)
 * and rectangular area lights (corner positions).
 */
struct Light
{
    /** @brief World-space position of the light */
    glm::vec3 position;
    
    /** @brief Light color (RGB) */
    glm::vec3 color;
    
    /** @brief Light intensity multiplier */
    float intensity;

    /** @brief Radius for sphere lights (soft shadows) */
    float radius;

    /**
     * @name Area Light Data
     * @brief Corner positions for rectangular area lights
     * @{
     */
    /** @brief First corner of area light rectangle */
    glm::vec3 cornerA;
    
    /** @brief Second corner of area light rectangle */
    glm::vec3 cornerB;
    /** @} */
};

#endif