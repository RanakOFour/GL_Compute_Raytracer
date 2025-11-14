#ifndef SPHERE_H
#define SPHERE_H

#include "GLM/glm.hpp"
#include <iostream>

#include "Model.h"

struct Sphere
{
    Model* model;
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
};

#endif