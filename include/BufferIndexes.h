#ifndef BUFFERINDEX_H
#define BUFFERINDEX_H

// I got really tired of having to constantly change the bind locations
// in Raytracer.cpp, so I centralised it here

#define GBUFFERCOUNT 4

enum BufferIndices
{
    OUTPUT_IMAGE = 0,

    // Gbuffers
    HITPOSITION = 1,
    HITNORMALS = 2,
    TEXMATINFO = 3,
    SHADOW = 4,

    // SSBO locations
    TRIANGLE_DATA = 5,
    BVH_NODES = 6,
    BVH_INDICES = 7,
    MATERIALS = 8,

    // Texture location
    TEXTURES = 9
};

#endif