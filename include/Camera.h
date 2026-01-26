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
    Camera(glm::vec2 _resolution);

    /** @brief Destructor */
    ~Camera();

    /**
     * @brief Store current state as "last frame" state
     * 
     * Should be called at the end of each frame to save current camera
     * state for use in temporal effects in the next frame.
     */
    void StoreLastFrameState();

    /**
     * @brief Update position and rotation using the Input struct
     * @param _inputMap Current frame input state
     * @param _deltaTime Time elapsed since last frame
     */
    void Update(Input _inputMap, float _deltaTime);

    /**
     * @brief Update shader with current and previous frame camera uniforms
     * @param _shader Reference to the compute shader to update
     * 
     * Sets the following uniforms:
     * - u_lastFrameCamera.position, forward, right, up, fov (previous frame)
     * - u_camera.position, forward, up, right, fov (current frame)
     */
    void UpdateShader(ComputeShader& _shader);

    /**
     * @name Setters and Getters
     * @{
     */

    /**
     * @brief Rotate camera around an axis
     * @param _angle Rotation angle in radians
     * @param _axis Axis to rotate around (normalized internally)
     */
    void Rotate(float _angle, glm::vec3 _axis);

    /**
     * @brief Move camera by offset in world space
     * @param _posChange Movement offset vector
     */
    void Move(glm::vec3 _posChange);
    
    /**
     * @brief Set camera position
     * @param _newPos New world position
     */
    void Position(glm::vec3 _newPos);

    /**
     * @brief Get current camera position
     * @return Reference to position vector
     */
    glm::vec3& Position();
    
    /**
     * @brief Get forward direction
     * @return Reference to forward vector
     */
    glm::vec3& Forward();
    
    /**
     * @brief Get right direction
     * @return Reference to right vector
     */
    glm::vec3& Right();
    
    /**
     * @brief Get up direction
     * @return Reference to up vector
     */
    glm::vec3& Up();
    
    /**
     * @brief Get rotation quaternion
     * @return Rotation quaternion
     */
    glm::quat Rotation();

    /**
     * @brief Set field of view
     * @param _f New FOV value
     */
    void fov(float _f);
    
    /**
     * @brief Get field of view
     * @return Current FOV value
     */
    float fov();
    /** @} */
};

#endif
