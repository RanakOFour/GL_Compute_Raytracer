#ifndef CAMERA_H
#define CAMERA_H

#include "GLM/ext.hpp"

class Camera
{
    private:
    glm::mat4 _viewingMatrice[3];
    glm::vec3 m_Position;
    glm::vec3 m_Forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    float m_fov;

    public:
    Camera()
    {
        _viewingMatrice[0] = glm::mat4(1.0f);
        _viewingMatrice[1] = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        _viewingMatrice[2] = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, -1.0f, 1.0f);
    };

    ~Camera()
    {};

    inline void FOV(float _fov) { m_fov = _fov; };
    inline float FOV() { return m_fov; };

    inline void Move(glm::vec3 _posChange) { m_Position += _posChange; };
    inline void Position(glm::vec3 _newPos) { m_Position = _newPos; };

    glm::vec3& Position() { return m_Position; }
    glm::vec3& Forward() { return m_Forward; };
    glm::vec3& Right() { return m_Right; };
    glm::vec3& Up() { return m_Up; };
};

#endif