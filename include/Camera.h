#ifndef CAMERA_H
#define CAMERA_H

#include "GLM/ext.hpp"
#include "ComputeShader.h"

/**
 *  @brief
*   Contains information on the scene camera.
*/
class Camera
{
    private:
    /**@{
    *  @name Properties
    */
    glm::vec3 m_Position;
    glm::quat m_Rotation;
    glm::vec3 m_Forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec2 m_Resolution;
    /**@} */

    public:
    Camera(glm::vec2 _resolution)
    : m_Resolution(_resolution)
    , m_Position(0.0, 0.0, 3.0)
    , m_Rotation(1.0f, 0.0f, 0.0f, 0.0f)
    {
    };

    ~Camera()
    {};

    /**
    *   @brief
    *   Updates all camera related shader uniforms
    */
    inline void UpdateShader(ComputeShader& _shader)
    {
        _shader.SetUniform("u_camera.position", m_Position);
        _shader.SetUniform("u_camera.forward", m_Forward);
        _shader.SetUniform("u_camera.up", m_Up);
        _shader.SetUniform("u_camera.right", m_Right);
    }

    /**
     * @{
     * @name Setters and Getters
     */

    inline void Rotate(float _angle, glm::vec3 _axis)
    {
        glm::quat rotation = glm::angleAxis(glm::radians(_angle), glm::normalize(_axis));

        m_Rotation = rotation * m_Rotation;
        m_Rotation = glm::normalize(m_Rotation);

        m_Forward = glm::normalize(m_Rotation * glm::vec3(0.0f, 0.0f, -1.0f));

        m_Right = glm::normalize(glm::cross(m_Forward, glm::vec3(0.0f, 1.0f, 0.0f)));

        m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
    };

    inline void Move(glm::vec3 _posChange) { m_Position += _posChange; };
    inline void Position(glm::vec3 _newPos) { m_Position = _newPos; };

    glm::vec3& Position() { return m_Position; };
    glm::vec3& Forward() { return m_Forward; };
    glm::vec3& Right() { return m_Right; };
    glm::vec3& Up() { return m_Up; };
    /** @} */
};

#endif