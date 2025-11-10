#ifndef SPHERE_H
#define SPHERE_H

#include "GLM/glm.hpp"
#include <iostream>

#include "CPURaytracer/Ray.h"

struct RaycastResult
{
    bool connect;
    glm::vec3 position;
    glm::vec3 normal;

    RaycastResult()
    {
        connect = false;
        position = glm::vec3(0);
        normal = position;
    }

    RaycastResult(bool _connect, glm::vec3 _pos, glm::vec3 _norm)
    {
        connect = _connect;
        position = _pos;
        normal = _norm;
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
        glm::vec3 l_rayToSphere = _ray->origin - position;
        float a = glm::dot(_ray->direction, _ray->direction);
        float b = 2.0f * glm::dot(l_rayToSphere, _ray->direction);
        float c = glm::dot(l_rayToSphere, l_rayToSphere) - radius * radius;

        // Builds a quadratic that calculates the distance from the sphere
        
        float discriminant = b * b - 4 * a * c;
        
        if (discriminant < 0) {
            // Ray is infront of the sphere
            return RaycastResult();
        }
        
        // Find the nearest intersection point
        float sqrtDiscriminant = glm::sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
        float t2 = (-b + sqrtDiscriminant) / (2.0f * a);
        
        // Use the closest positive intersection
        float t = (t1 > 0) ? t1 : t2;
        if (t <= 0) {
            return RaycastResult(); // Intersection behind ray origin
        }
        
        glm::vec3 intersectPoint = _ray->origin + t * _ray->direction;
        return RaycastResult(true, intersectPoint, GetNormal(intersectPoint));
    }

    glm::vec3 Shade(glm::vec3 _intersectPoint)
    {
        glm::vec3 lightDir = glm::normalize(glm::vec3(0, -1, 0)); // example light direction
        glm::vec3 normal = GetNormal(_intersectPoint);

        float diffuse = glm::max(glm::dot(normal, lightDir), 0.0f);

        float ambient = 0.1f;

        return colour * (ambient + (1.0f - ambient) * diffuse);
    }

    glm::vec3 GetNormal(glm::vec3 _point)
    {
        return glm::normalize(_point - position);
    }
};

#endif