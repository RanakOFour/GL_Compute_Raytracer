#ifndef RAY_H
#define RAY_H
#include "GLM/glm.hpp"

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;

    Ray()
    {
        origin = glm::vec3(0);
        direction = glm::vec3(0);
    }

    Ray(glm::vec3 _o, glm::vec3 _d) :
    origin(_o),
    direction(_d)
    { }
};


#endif