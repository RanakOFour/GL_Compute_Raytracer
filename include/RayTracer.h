#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "GLM/glm.hpp"
#include <vector>

#include "Ray.h"
#include "Sphere.h"

class RayTracer
{
    private:
    std::vector<Sphere> _objects;

    public:
    RayTracer() :
    _objects()
    {
        _objects.push_back(Sphere(glm::vec3(0, 0, 1), 0.5f, glm::vec3(1, 0, 0)));
    };
    ~RayTracer()
    {};

    glm::vec3 TraceRay(Ray* _ray)
    {
        glm::vec3 toReturn = glm::vec3(0);
        int l_sphereCount = _objects.size();
        RaycastResult result;


        for(int i = 0; i < l_sphereCount; ++i)
        {
            result = _objects[i].RayIntersect(_ray);
            if(result.connect)
            {
                toReturn = _objects[i].colour;
            }
        }

        return toReturn;
    }
};

#endif