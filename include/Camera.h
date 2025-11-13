#ifndef CAMERA_H
#define CAMERA_H

#include "GLM/ext.hpp"
#include "ComputeShader.h"

class Camera
{
    private:
    glm::vec3 m_Position;
    glm::vec3 m_Forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec2 m_Resolution;
    glm::vec2 m_viewportResolution;

    float m_aspectRatio;
    float m_fov;
    float m_focalLength;

    public:
    Camera(glm::vec2 _resolution) :
    m_Resolution(_resolution)
    {
        m_aspectRatio = _resolution.x / _resolution.y;
        m_viewportResolution = glm::vec2(0.0);
        m_viewportResolution.y = 2.0;
        m_viewportResolution.x = m_viewportResolution.y * m_aspectRatio;
        m_focalLength = 1.0f;

        m_Position = glm::vec3(0.0, 0.0, -3.0);
    };

    ~Camera()
    {};

    inline void SetShaderValues(ComputeShader* _shader)
    {
        _shader->SetUniform("u_camera.position", m_Position);
        _shader->SetUniform("u_camera.forward", m_Forward);
        _shader->SetUniform("u_camera.up", m_Up);
        _shader->SetUniform("u_camera.right", m_Right);
        _shader->SetUniform("u_camera.fov", m_fov);
    }

    inline void FOV(float _fov) { m_fov = _fov; };
    inline float FOV() { return m_fov; };

    inline void FocalLength(float _fl) { m_focalLength = _fl; };
    inline float FocalLength() {return m_focalLength; };

    inline void Move(glm::vec3 _posChange) { m_Position += _posChange; };
    inline void Position(glm::vec3 _newPos) { m_Position = _newPos; };

    glm::vec3& Position() { return m_Position; };
    glm::vec3& Forward() { return m_Forward; };
    glm::vec3& Right() { return m_Right; };
    glm::vec3& Up() { return m_Up; };
};

#endif