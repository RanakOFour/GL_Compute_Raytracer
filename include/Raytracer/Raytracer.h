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
#include "Raytracer/BufferIndexes.h"

#include "Datastructs/Camera.h"
#include "Datastructs/Triangle.h"
#include "Datastructs/Material.h"
#include "Datastructs/Texture.h"
#include "Datastructs/Light.h"

#include "Shader/ShaderInfo.h"

#include <vector>

class GUI;
class Camera;

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

    /**
     * @name Scene Data Pointers
     * @brief External scene data (allows hooking raytracer to different data sources)
     * @{
     */
    /** @brief Pointer to triangle geometry data */
    std::vector<Triangle>* m_tris;
    
    /** @brief Pointer to material data */
    std::vector<Material>* m_mats;
    
    /** @brief Pointer to texture data */
    std::vector<Texture>* m_textures;
    /** @} */

    /** @brief Scene lights managed by the raytracer */
    std::vector<Light> m_lights;

    /** @brief Scene camera for view generation */
    Camera m_camera;

    /** @brief Collection of compute shaders used in the pipeline */
    std::vector<std::shared_ptr<ShaderInfo>>* m_shaders;
    
    /** @brief G-buffer texture handles for deferred rendering */
    std::vector<GLuint> m_gBuffers;

    /** @brief SSBO for triangle data on GPU */
    GLuint m_triangleSSBO;
    
    /** @brief SSBO for material data on GPU */
    GLuint m_materialSSBO;

    /** @brief Texture unit location for the main output texture */
    GLuint m_mainTextureLoc;

    /** @brief Render resolution (width, height) */
    glm::ivec2 m_renderSize;

    /** @brief Flag indicating if the raytracer has been properly initialized */
    bool m_setup;

    /** @brief Current frame counter for temporal effects */
    int m_frameCount;

    /**
     * @brief Setup G-buffers and other GPU resources
     */
    inline void BuildRenderDataBuffers();

public:
    /**
     * @brief Construct a new Raytracer
     * @param _screenSize Render resolution (width, height)
     * @param _mainTextureLoc Texture unit for the output image
     */
    Raytracer(glm::ivec2 _screenSize, GLuint _mainTextureLoc);
    
    /** @brief Destructor - releases GPU resources */
    ~Raytracer();

    /**
     * @brief Execute a frame of raytracing
     * @param _time Current time for animated effects
     * 
     * Runs all enabled compute shaders in order, updating uniforms
     * and dispatching compute work groups.
     */
    void Trace(float _time);

    /**
     * @brief Set the triangle geometry source
     * @param _tris Pointer to triangle vector
     */
    void SetTris(std::vector<Triangle>* _tris);
    
    /**
     * @brief Set the materials source
     * @param _mat Pointer to material vector
     */
    void SetMaterials(std::vector<Material>* _mat);
    
    /**
     * @brief Set the textures source
     * @param _tex Pointer to texture vector
     */
    void SetTextures(std::vector<Texture>* _tex);
    
    /**
     * @brief Add a light to the scene
     * @param _light Light to add
     */
    void AddLight(Light _light);
    
    /**
     * @brief Get a light by index
     * @param index Light index
     * @return Pointer to the light, or nullptr if invalid index
     */
    Light* GetLight(int index);

    /**
     * @brief Get a material by index
     * @param _index Material index
     * @return Pointer to the material, or nullptr if invalid index
     */
    Material* GetMaterial(int _index);

    /**
     * @brief Get reference to the camera
     * @return Reference to the camera object
     */
    Camera& GetCamera() { return m_camera; }

    /**
     * @brief Get reference to the lights vector
     * @return Reference to the lights vector
     */
    std::vector<Light>& GetLights() { return m_lights; }

    /**
     * @brief Get reference to the materials vector
     * @return Reference to the materials vector (from external pointer)
     */
    std::vector<Material>& GetMaterials() { return *m_mats; }
};

#endif