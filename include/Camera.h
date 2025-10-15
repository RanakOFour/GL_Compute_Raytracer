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
        
    };
    ~Camera()
    {};

    Ray GetRay(glm::ivec2 _screenPosition)
    {
        Ray toReturn(glm::vec3(((2.0f * (float)_screenPosition.x) / 640.0f) - 1.0f, 1.0f - (2.0f * (float)_screenPosition.y) / 480.0f, 0),
                     glm::vec3(0, 0, -1));

        return toReturn;
    }
};

#endif