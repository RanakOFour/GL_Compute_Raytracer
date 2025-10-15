#ifndef CAMERA_H
#define CAMERA_H

#include "GLM/ext.hpp"
#include "Ray.h"

class Camera
{
    private:
    glm::mat4 _viewingMatrice[3];

    public:
    Camera()
    {
        _viewingMatrice[0] = glm::mat4(1.0f);
        _viewingMatrice[1] = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        _viewingMatrice[2] = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, -1.0f, 1.0f);
    };

    ~Camera()
    {};

    Ray GetRay(glm::ivec2 _screenPosition)
    {
        float x = (2 * ((float)_screenPosition.x / 640.0f)) - 1.0f;
        float y = 1.0f - (2 * ((float)_screenPosition.y / 480.0f));

        glm::vec4 l_clipSpaceRay = glm::vec4(x, y, -1, 1);

        glm::vec4 l_eyeSpaceRay= glm::inverse(_viewingMatrice[2]) * l_clipSpaceRay;
        l_eyeSpaceRay[2] = -1.0f;
        l_eyeSpaceRay[3] = 0.0f;

        glm::vec3 l_worldSpaceRay = glm::vec3(glm::inverse(_viewingMatrice[1]) * l_eyeSpaceRay);
        l_worldSpaceRay = glm::normalize(l_worldSpaceRay);

        Ray toReturn(glm::vec3(0, 0, 1), l_worldSpaceRay);

        return toReturn;
    }
};

#endif