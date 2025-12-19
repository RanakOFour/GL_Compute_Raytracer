#ifndef BUFFERINDEX_H
#define BUFFERINDEX_H

// I got really tired of having to constantly change the bind locations
// in Raytracer.cpp, so I centralised it here

#define GBUFFERCOUNT 6

enum BufferIndices
{
    OUTPUT_IMAGE,

    // Gbuffers
    HITPOSITION,
    HITNORMALS,
    TEXMATINFO,
    SHADOW,
    PREVFRAMEINFO,
    MOTIONVECTORS,

    // SSBO locations
    TRIANGLE_DATA,
    BVH_NODES,
    BVH_INDICES,
    MATERIALS,

    // Texture location
    TEXTURES
};

#endif