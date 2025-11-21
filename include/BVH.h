#ifndef BVH_H
#define BVH_H

#include <vector>
#include <iostream>

#include "GL/glew.h"
#include "GLM/glm.hpp"

#include "Triangle.h"
#include "AABB.h"

/**
 * @details
 * A Bounding Volume Hierarchy that groups triangles
 * into a tree structure to accelerate Ray-Tracing processes
 */
class BVH
{
    private:
    GLuint m_nodeSSBOID;
    GLuint m_indexesSSBOID;
    GLuint m_triangleSSBOID;

    /**@{ 
     * @name Dirty flags
     * Dirty flags for denoting when data should
     * be reuploaded to the GPU
    */
    bool m_dirtyNodes;
    bool m_dirtyIDs;
    bool m_dirtyTris;
    /**@} */

    // Bit alignment hocus pocus
    struct Node
    {
        glm::vec3 minBound;
        int leftFirst;
        glm::vec3 maxBound;
        int triangleCount;
    };

    std::vector<Node> m_nodes;
    std::vector<Triangle> m_tris;
    std::vector<int> m_triIndexes;

    void UpdateNodeBounds(int _nodeIndex);

    float CalculateSAH(Node& _node, int _axis, float _pos);

    void Subdivide(int _nodeIndex);

    public:
    BVH(std::vector<Triangle>* _tris)
    : m_nodeSSBOID(0)
    , m_indexesSSBOID(0)
    , m_triangleSSBOID(0)
    , m_nodes()
    , m_tris(*_tris)
    , m_triIndexes()
    , m_dirtyNodes(true)
    , m_dirtyIDs(true)
    , m_dirtyTris(true)
    {
        BuildBHV();
    };

    ~BVH()
    {
        if (m_indexesSSBOID)
        {
            glDeleteBuffers(1, &m_indexesSSBOID);
        }

        if (m_nodeSSBOID)
        {
            glDeleteBuffers(1, &m_nodeSSBOID);
        }

        if (m_triangleSSBOID)
        {
            glDeleteBuffers(1, &m_triangleSSBOID);
        }
    };

    void BuildBHV();

    GLuint GetTriangleSSBO();

    GLuint GetIndexSSBO();

    GLuint GetNodeSSBO();
};

#endif