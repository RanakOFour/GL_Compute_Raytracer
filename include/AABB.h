/**
 * @file AABB.h
 * @brief Axis-Aligned Bounding Box structure for spatial acceleration
 * 
 * This file provides the AABB (Axis-Aligned Bounding Box) structure used
 * for BVH construction and ray intersection acceleration in the raytracer.
 */

#ifndef AABB_H
#define AABB_H

#include "GLM/ext.hpp"

/**
 * @struct AABB
 * @brief Axis-Aligned Bounding Box defined by minimum and maximum extents
 * 
 * A set of half-extents that describe a 3D cube area aligned to the world axes.
 * Used for spatial partitioning and acceleration structures in raytracing.
 */
struct AABB
{
    /** @brief Minimum corner of the bounding box */
    glm::vec3 bmin;
    
    /** @brief Maximum corner of the bounding box */
    glm::vec3 bmax;

    /**
     * @brief Default constructor creating an invalid (inverted) bounding box
     * 
     * Initializes bmin to a very large value and bmax to a very small value,
     * allowing Grow() to work correctly from the first point added.
     */
    AABB() :
    bmin(1e30f),
    bmax(-1e30f)
    {
    }

    /**
     * @brief Expand the bounding box to contain the given point
     * @param _p Point to include in the bounding box
     */
    inline void Grow(glm::vec3& _p)
    {
        bmin = glm::min(bmin, _p);
        bmax = glm::max(bmax, _p);
    };

    /**
     * @brief Calculate the surface area of the bounding box
     * @return Surface area of the box (used for SAH calculations)
     * 
     * Calculates the area of the cube described by the half extents
     * bmin and bmax. Used in Surface Area Heuristic (SAH) for BVH construction.
     */
    inline float Area()
    {
        glm::vec3 e = bmax - bmin;
        return e.x * e.y + e.y * e.z + e.z * e.x;
    };
};

#endif