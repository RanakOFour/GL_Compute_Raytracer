/**
 * @file Raytracer.h
 * @brief Main raytracer class managing scene rendering
 * 
 * This file provides the Raytracer class which manages the compute shader
 * pipeline, scene geometry, materials, lights, and camera for GPU-accelerated
 * path tracing.
 */

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "GLM/ext.hpp"

#include "Raytracer/BVH.h"

#include "Datastructs/Camera.h"
#include "Datastructs/Triangle.h"
#include "Datastructs/Material.h"
#include "Datastructs/Texture.h"
#include "Datastructs/Light.h"

#include "Shader/ShaderInfo.h"
#include "ShaderStorageBuffer.h"

#include <vector>

class GUI;
class Camera;
class Window;

/**
 * @class Raytracer
 * @brief Core raytracing engine managing shaders, geometry, and rendering
 * 
 * Handles the GPU-accelerated raytracing pipeline including BVH construction,
 * shader management, G-buffer setup, and frame rendering.
 */
class Raytracer
{
    friend GUI;

private:
    /** @brief Bounding Volume Hierarchy for ray acceleration */
    BVH m_BVH;

    /** @brief Scene camera for view generation */
    Camera m_camera;

    /** @brief Collection of compute shaders used in the pipeline */
    std::vector<std::shared_ptr<ShaderInfo>>* m_shaders;
    
    /** @brief G-buffer texture handles for deferred rendering */
    std::vector<GLuint> m_gBuffers;

    std::vector<std::shared_ptr<IShaderStorageBuffer>> m_ssbos;

    std::weak_ptr<Window> m_windowPtr;

    /** @brief Flag indicating if the raytracer has been properly initialized */
    bool m_setup;

    /** @brief Current frame counter for temporal effects */
    int m_frameCount;

    inline void BuildRenderDataBuffers();

public:
    /**
     * @brief Construct a new Raytracer
     * @param _screenSize Render resolution (width, height)
     * @param _mainTextureLoc Texture unit for the output image
     */
    Raytracer(std::weak_ptr<Window> _windowPtr);
    
    /** @brief Destructor - releases GPU resources */
    ~Raytracer();

    /**
     * @brief Setup G-buffers and other GPU resources
     */
    void RebuildGBuffers();

    /**
     * @brief Execute a frame of raytracing
     * @param _time Current time for animated effects
     * 
     * Runs all enabled compute shaders in order, updating uniforms
     * and dispatching compute work groups.
     */
    void Trace(float _time);

    void AddSSBO(std::shared_ptr<IShaderStorageBuffer> _ssbo);
    void GetSSBO(int l_ssboID);
    
    void BuildBVH(std::vector<Triangle>* _tris);
    void BuildBVH(int l_ssboID);

    /**
     * @brief Get reference to the camera
     * @return Reference to the camera object
     */
    Camera& GetCamera() { return m_camera; }
};

#endif