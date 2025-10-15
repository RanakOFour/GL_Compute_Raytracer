#ifndef SPHERE_H
#define SPHERE_H

#include "GLM/glm.hpp"
#include <iostream>

#include "Ray.h"

struct RaycastResult
{
    bool connect;
    glm::vec3 position;

    RaycastResult()
    {
        connect = false;
        position = glm::vec3(0);
    }

    RaycastResult(bool _connect, glm::vec3 _pos)
    {
        connect = _connect;
        position = _pos;
    }
};

struct Sphere
{
    glm::vec3 position;
    float radius;
    glm::vec3 colour;

    Sphere() :
    position(0),
    radius(1),
    colour(1)
    {

    }

    Sphere(glm::vec3 _p, float _r, glm::vec3 _c) :
    position(_p),
    radius(_r),
    colour(_c)
    {}

    RaycastResult RayIntersect(Ray* _ray)
    {
        //Check if inside
        float l_distance = glm::pow(_ray->origin.x - position.x, 2) + glm::pow(_ray->origin.y - position.y, 2) + glm::pow(_ray->origin.z - position.z, 2);
        float l_radiusSquare = radius * radius;

        if(l_distance < l_radiusSquare)
        {
            // Inside sphere, so no collision
            //printf("Distance issue\n");
            return RaycastResult();
        }

        //Check for intersect

        float rayToSphereDot = glm::dot(position - _ray->origin, _ray->direction);

        float l_d = (position - _ray->origin - ((rayToSphereDot * _ray->direction) * _ray->direction)).length();

        if(l_d > radius)
        {
            // No collision
            printf("D issue\n");
            return RaycastResult();
        }

        float l_x = glm::sqrt((radius * radius) - (l_d * l_d));

        glm::vec3 l_closestIntersect = _ray->origin + (rayToSphereDot - l_x) * _ray->direction;

        printf("Intersect\n");
        return RaycastResult(true, l_closestIntersect);
    }

    glm::vec3 Shade()
    {
        return colour;
    }

    glm::vec3 GetNormal(glm::vec3 _point)
    {
        return glm::vec3(0);
    }
};

#endif