/**
 * @file Camera.h
 * @brief Camera class for 3D scene navigation with temporal state tracking
 * 
 * This file provides the Camera class which manages camera position, orientation,
 * field of view, and updates shader uniforms for both current and previous frame
 * camera states (useful for temporal effects like motion blur).
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "GLM/ext.hpp"
#include "ComputeShader.h"
#include "Input.h"

/**
 * @class Camera
 * @brief Manages camera state and shader uniform updates for raytracing
 * 
 * Provides functionality for camera movement, rotation, and shader uniform
 * synchronization. Tracks both current and previous frame camera state
 * for temporal effects in the raytracer.
 */
class Camera
{
private:
    /**
     * @name Camera Properties
     * @{
     */
    /** @brief Current camera position in world space */
    glm::vec3 m_Position;
    
    /** @brief Camera orientation as quaternion */
    glm::quat m_Rotation;
    
    /** @brief Camera forward direction vector */
    glm::vec3 m_Forward = glm::vec3(0.0f, 0.0f, -1.0f);
    
    /** @brief Camera right direction vector */
    glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
    
    /** @brief Camera up direction vector */
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    /** @brief Screen resolution for aspect ratio calculations */
    glm::vec2 m_Resolution;

    /** @brief Vertical field of view */
    float m_fov;
    /** @} */

    /**
     * @name Previous Frame State
     * @brief State from the last frame for temporal effects
     * @{
     */
    /** @brief Previous frame camera position */
    glm::vec3 m_LastPosition;
    
    /** @brief Previous frame forward vector */
    glm::vec3 m_LastForward;
    
    /** @brief Previous frame right vector */
    glm::vec3 m_LastRight;
    
    /** @brief Previous frame up vector */
    glm::vec3 m_LastUp;
    
    /** @brief Previous frame field of view */
    float m_LastFov;
    /** @} */

public:
    /**
     * @brief Construct a new Camera object with specified resolution
     * @param _resolution Screen resolution for aspect ratio calculations
     */
    Camera(glm::vec2 _resolution)
        : m_Resolution(_resolution)
        , m_Position(0.0, 0.0, 3.0)
        , m_Rotation(1.0f, 0.0f, 0.0f, 0.0f)
        , m_fov(0.5f)
        , m_LastPosition(0.0, 0.0, 3.0)
        , m_LastForward(0.0f, 0.0f, -1.0f)
        , m_LastRight(1.0f, 0.0f, 0.0f)
        , m_LastUp(0.0f, 1.0f, 0.0f)
        , m_LastFov(0.5f)
    {
    };

    /** @brief Destructor */
    ~Camera()
    {};

    /**
     * @brief Store current state as "last frame" state
     * 
     * Should be called at the end of each frame to save current camera
     * state for use in temporal effects in the next frame.
     */
    inline void StoreLastFrameState()
    {
        m_LastPosition = m_Position;
        m_LastForward = m_Forward;
        m_LastRight = m_Right;
        m_LastUp = m_Up;
        m_LastFov = m_fov;
    }

    /**
     * @brief Update position and rotation using the Input struct
     * @param _inputMap Current frame input state
     * @param _deltaTime Time elapsed since last frame
     */
    inline void Update(Input _inputMap, float _deltaTime)
    {
        Move(_inputMap.forward * m_Forward * 3.0f * _deltaTime);
        Move(_inputMap.right * m_Right * 3.0f * _deltaTime);
        Move(_inputMap.up * m_Up * 3.0f * _deltaTime);

        if(abs(_inputMap.deltaMouseX) > 0.0f && abs(_inputMap.deltaMouseX) < 1.0f)
        {
            Rotate(_inputMap.deltaMouseX * 10.0f * _deltaTime, m_Up);
        }

        if(abs(_inputMap.deltaMouseY) > 0.0f && abs(_inputMap.deltaMouseY) < 1.0f)
        {
            Rotate(_inputMap.deltaMouseY * 10.0f * _deltaTime, m_Right);
        }
    };

    /**
     * @brief Update shader with current and previous frame camera uniforms
     * @param _shader Reference to the compute shader to update
     * 
     * Sets the following uniforms:
     * - u_lastFrameCamera.position, forward, right, up, fov (previous frame)
     * - u_camera.position, forward, up, right, fov (current frame)
     */
    inline void UpdateShader(ComputeShader& _shader)
    {
        // Set last frame camera uniforms
        _shader.SetUniform("u_lastFrameCamera.position", m_LastPosition);
        _shader.SetUniform("u_lastFrameCamera.forward", m_LastForward);
        _shader.SetUniform("u_lastFrameCamera.right", m_LastRight);
        _shader.SetUniform("u_lastFrameCamera.up", m_LastUp);
        _shader.SetUniform("u_lastFrameCamera.fov", m_LastFov);

        // Set current camera uniforms
        _shader.SetUniform("u_camera.position", m_Position);
        _shader.SetUniform("u_camera.forward", m_Forward);
        _shader.SetUniform("u_camera.up", m_Up);
        _shader.SetUniform("u_camera.right", m_Right);
        _shader.SetUniform("u_camera.fov", m_fov);
    }

    /**
     * @name Setters and Getters
     * @{
     */

    /**
     * @brief Rotate camera around an axis
     * @param _angle Rotation angle in radians
     * @param _axis Axis to rotate around (normalized internally)
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

    /**
     * @brief Move camera by offset in world space
     * @param _posChange Movement offset vector
     */
    inline void Move(glm::vec3 _posChange) { m_Position += _posChange; };
    
    /**
     * @brief Set camera position
     * @param _newPos New world position
     */
    inline void Position(glm::vec3 _newPos) { m_Position = _newPos; };

    /**
     * @brief Get current camera position
     * @return Reference to position vector
     */
    inline glm::vec3& Position() { return m_Position; };
    
    /**
     * @brief Get forward direction
     * @return Reference to forward vector
     */
    inline glm::vec3& Forward() { return m_Forward; };
    
    /**
     * @brief Get right direction
     * @return Reference to right vector
     */
    inline glm::vec3& Right() { return m_Right; };
    
    /**
     * @brief Get up direction
     * @return Reference to up vector
     */
    inline glm::vec3& Up() { return m_Up; };
    
    /**
     * @brief Get rotation quaternion
     * @return Rotation quaternion
     */
    inline glm::quat Rotation() { return m_Rotation; };

    /**
     * @brief Set field of view
     * @param _f New FOV value
     */
    inline void fov(float _f) { m_fov = _f; };
    
    /**
     * @brief Get field of view
     * @return Current FOV value
     */
    inline float fov() { return m_fov; };
    /** @} */
};

#endif
