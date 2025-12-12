#ifndef CAMERA_H
#define CAMERA_H

#include "GLM/ext.hpp"
#include "ComputeShader.h"
#include "Input.h"

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

    float m_fov;
    /**@} */

    public:
    Camera(glm::vec2 _resolution)
    : m_Resolution(_resolution)
    , m_Position(0.0, 0.0, 3.0)
    , m_Rotation(1.0f, 0.0f, 0.0f, 0.0f)
    , m_fov(0.5f)
    {
    };

    ~Camera()
    {};

    /**
     * @brief
     * Update position and rotation using the Input struct
     */
    inline void Update(Input _inputMap, float _deltaTime)
    {
        // You do not wish to know how many tries it took me to get this somewhat reasonable
        // This is also platform specific, so I hope that it works on windows
        // Apparently, SDL timers can work to the nearest nanosecond (x10^9),
        // which is odd considering the value below (1x10^18) implies something much worse

        
        float l_moveDelta = 0.0f;
        
        #if _WIN32
        l_moveDelta = _deltaTime / 10000000000000.0f;
        #else
        l_moveDelta = 0.1f;
        #endif

		Move(_inputMap.forward * m_Forward * (l_moveDelta));

		Move(_inputMap.right * m_Right * (l_moveDelta));

        Move(_inputMap.up * m_Up * (l_moveDelta));

        if(abs(_inputMap.deltaMouseX) > 0.0f && abs(_inputMap.deltaMouseX) < 1.0f)
        {
            Rotate(_inputMap.deltaMouseX * (l_moveDelta), m_Up);
        }

        if(abs(_inputMap.deltaMouseY) > 0.0f && abs(_inputMap.deltaMouseY) < 1.0f)
        {
            Rotate(_inputMap.deltaMouseY * (l_moveDelta), m_Right);
        }

        //printf("%i, %i, %i, %f, %f\n", _inputMap.forward, _inputMap.right, _inputMap.up, _inputMap.deltaMouseX, _inputMap.deltaMouseY);
    };

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
        _shader.SetUniform("u_camera.fov", m_fov);
    }

    /**
     * @{
     * @name Setters and Getters
     */

    inline void Rotate(float _angle, glm::vec3 _axis)
    {
        glm::quat rotation = glm::angleAxis(_angle, glm::normalize(_axis));

        m_Rotation = rotation * m_Rotation;
        m_Rotation = glm::normalize(m_Rotation);

        m_Forward = glm::normalize(m_Rotation * glm::vec3(0.0f, 0.0f, -1.0f));

        m_Right = glm::normalize(glm::cross(m_Forward, glm::vec3(0.0f, 1.0f, 0.0f)));

        m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
    };

    inline void Move(glm::vec3 _posChange) { m_Position += _posChange; };
    inline void Position(glm::vec3 _newPos) { m_Position = _newPos; };

    inline glm::vec3& Position() { return m_Position; };
    inline glm::vec3& Forward() { return m_Forward; };
    inline glm::vec3& Right() { return m_Right; };
    inline glm::vec3& Up() { return m_Up; };
    inline glm::quat Rotation() {return m_Rotation;};

    inline void fov(float _f) { m_fov = _f; };
    inline float fov() { return m_fov; };
    /** @} */
};

#endif