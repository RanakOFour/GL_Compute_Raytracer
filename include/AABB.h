#ifndef AABB_H
#define AABB_H

#include "GLM/ext.hpp"

struct AABB
{
    glm::vec3 bmin;
    glm::vec3 bmax;

    AABB() :
    bmin(1e30f),
    bmax(-1e30f)
    {

    }

    inline void Grow(glm::vec3& _p)
    {
        bmin = glm::min(bmin, _p);
        bmax = glm::max(bmax, _p);
    };

    inline float Area()
    {
        glm::vec3 e = bmax - bmin;
        return e.x * e.y + e.y * e.z + e.z * e.x;
    };
};

#endif