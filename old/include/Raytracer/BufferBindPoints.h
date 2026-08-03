/**
 * @file BufferIndexes.h
 * @brief Centralized definitions for OpenGL buffer binding locations
 * 
 * This file provides a single location for all buffer binding indices used
 * throughout the raytracer, avoiding the need to update multiple files
 * when changing binding locations.
 */

#ifndef BUFFERINDEX_H
#define BUFFERINDEX_H

/** @brief Number of G-buffers used in deferred rendering passes */
#define GBUFFERCOUNT 6
#define SSBOCOUNT 6

/**
 * @enum BufferIndices
 * @brief Binding locations for all OpenGL buffers and images
 * 
 * Defines the binding points for image units, SSBOs, and texture units
 * used by the raytracer's compute shaders.
 */

enum BufferBindPoints
{
    /** @brief Output render target image */
    OUTPUT_IMAGE,

    /**
     * @name G-Buffer Binding Locations
     * @brief Binding points for deferred rendering G-buffers
     * @{
     */
    /** @brief World-space hit position buffer */
    HITPOSITION,
    
    /** @brief Surface normal buffer */
    HITNORMALS,
    
    /** @brief Texture and material information buffer */
    TEXMATINFO,
    
    /** @brief Shadow mask buffer */
    SHADOW,
    
    /** @brief Previous frame information for temporal effects */
    PREVFRAMEINFO,
    
    /** @brief Motion vectors for temporal effects */
    MOTIONVECTORS,
    /** @} */

    /**
     * @name SSBO Binding Locations
     * @brief Binding points for Shader Storage Buffer Objects
     * @{
     */
    /** @brief Triangle geometry data SSBO */
    TRIANGLE_DATA,
    
    /** @brief BVH node data SSBO */
    BVH_NODES,
    
    /** @brief BVH triangle index SSBO */
    BVH_INDICES,
    
    /** @brief Material data SSBO */
    MATERIALS,

    /** @brief Texture data SSBO */
    TEXTURES,

    /** @brief Light data SSBO */
    LIGHTS
    /** @} */
};

#endif