#ifndef BVH_H
#define BVH_H

#include <vector>

#include "GL/glew.h"
#include "GLM/glm.hpp"

#include "Triangle.h"

struct BVH
{
    private:
    GLuint m_ssboID;

    struct Node
    {
        glm::vec3 minBound;
        float _padding_min;
        glm::vec3 maxBound;
        float _padding_max;
        
        // Useing ints instead of pointers for parity with OpenGL struct
        int leftNodeIndex;
        int rightNodeIndex;

        bool isLeaf;

        std::vector<Triangle> _tris;
    };

    std::vector<Node> m_nodes;

    public:
    BVH() {};
    ~BVH() {};
};

#endif