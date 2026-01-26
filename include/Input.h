/**
 * @file Input.h
 * @brief Input state structure for keyboard and mouse input
 * 
 * This file provides the Input struct which stores the current state
 * of user input for camera movement and rotation.
 */

#ifndef INPUT_H
#define INPUT_H

/**
 * @struct Input
 * @brief Stores the current state of user input
 * 
 * Contains directional movement inputs and mouse delta values
 * for controlling the camera in the raytracer.
 */
struct Input
{
    /** @brief Forward/backward movement input (-1, 0, or 1) */
    int forward;
    
    /** @brief Right/left movement input (-1, 0, or 1) */
    int right;
    
    /** @brief Up/down movement input (-1, 0, or 1) */
    int up;
    
    /** @brief Mouse horizontal movement delta this frame */
    float deltaMouseX;
    
    /** @brief Mouse vertical movement delta this frame */
    float deltaMouseY;

    /**
     * @brief Default constructor initializing all inputs to zero
     */
    Input()
    : forward(0)
    , right(0)
    , up(0)
    , deltaMouseX(0.0f)
    , deltaMouseY(0.0f)
    {};
};

#endif