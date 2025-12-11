#ifndef BUFFERINDEX_H
#define BUFFERINDEX_H

// I got really tired of having to constantly change the bind locations
// in Raytracer.cpp, so I centralised it here

#define GBUFFERCOUNT 5

#define OUTPUT_IMAGE_LOC 0

// Gbuffers
#define HITLOCATIONS_BUFFER_LOC 1
#define NORMALS_BUFFER_LOC 2
#define TEXMATINFO_BUFFER_LOC 3
#define SHADOW_BUFFER_LOC 4
#define LIGHT_HIT_BUFFER 5

// Locations to bind SSBOs to
#define TRIANGLEDATA_BUFFER_LOC 6
#define BVHNODE_BUFFER_LOC 7
#define BVHINDEX_BUFFER_LOC 8
#define MATERIAL_BUFFER_LOC 9

// start of texture uniform
#define TEXTURE_BUFFER_LOC 10


#endif