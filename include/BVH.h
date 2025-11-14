#ifndef BVH_H
#define BVH_H

#include "GL/glew.h"
#include "GLM/glm.hpp"

struct BVH
{
    private:
    GLuint m_ssboID;

    public:
    BVH() {};
    ~BVH() {};
};

#endif