#ifndef AABB_H
#define AABB_H

#include "GLM/ext.hpp"

/**
 * @brief
 * A set of half-extents that describe a 3D cube area
 */
struct AABB
{
    glm::vec3 bmin;
    glm::vec3 bmax;

    AABB() :
    bmin(1e30f),
    bmax(-1e30f)
    {

    }

    /**
     * @brief
     * Changes the size of the AABB to contain the point passed in
     */
    inline void Grow(glm::vec3& _p)
    {
        bmin = glm::min(bmin, _p);
        bmax = glm::max(bmax, _p);
    };


    /**
     * @brief
     * Calculates the area of the Cube described by the half extents
     * AABB::bmin and AABB::bmax
     */
    inline float Area()
    {
        glm::vec3 e = bmax - bmin;
        return e.x * e.y + e.y * e.z + e.z * e.x;
    };
};

#endif