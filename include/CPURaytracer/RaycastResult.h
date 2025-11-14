#ifndef RAYCAST_RESULT_H
#define RAYCAST_RESULT_H

#include "GLM/glm.hpp"



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

#endif