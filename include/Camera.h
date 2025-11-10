#ifndef CAMERA_H
#define CAMERA_H

#include "GLM/ext.hpp"
#include "ComputeShader.h"

class Camera
{
    private:
    glm::vec3 m_Position;
    glm::vec3 m_Forward = glm::vec3(0.0f, 0.0f, 1.0f);
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

        m_Position = glm::vec3(3.0, 0.0, 3.0);
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
        _shader->SetUniform("u_camera.aspect", m_aspectRatio);
        _shader->SetUniform("u_camera.viewportHeight", m_viewportResolution.y);
        _shader->SetUniform("u_camera.viewportWidth", m_viewportResolution.x);

        _shader->SetUniform("u_camera.resolution", m_Resolution);
        _shader->SetUniform("u_camera.focalLength", m_focalLength);

        glm::vec3 viewport_u = glm::vec3(m_viewportResolution.x, 0, 0);
        _shader->SetUniform("u_camera.viewport_u", viewport_u);

        glm::vec3 viewport_v = glm::vec3(0, m_viewportResolution.y, 0);
        _shader->SetUniform("u_camera.viewport_v", viewport_v);

        glm::vec3 pixelDeltaU = viewport_u / m_Resolution.x;
        _shader->SetUniform("u_camera.pixel_delta_u", pixelDeltaU);

        glm::vec3 pixelDeltaV = viewport_v / m_Resolution.y;
        _shader->SetUniform("u_camera.pixel_delta_v", pixelDeltaV);

        glm::vec3 vUpperLeft = m_Position - glm::vec3(0, 0, m_focalLength) - viewport_u * 0.5f - viewport_v * 0.5f;

        _shader->SetUniform("u_camera.viewport_upper_left", vUpperLeft);
        _shader->SetUniform("u_camera.pixel00location", vUpperLeft + 0.5f * (pixelDeltaU + pixelDeltaV));
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