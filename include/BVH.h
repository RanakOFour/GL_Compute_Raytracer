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
    bool m_dirtyIdxs;
    bool m_dirtyTris;
    /**@} */

    // Bit alignment hocus pocus
    struct BVHNode
    {
        glm::vec3 minBound;
        int leftFirst;
        glm::vec3 maxBound;
        int triangleCount;
    };

    /**
     * @brief
     * The list of raw triangles that the BVH will sort into a tree
     */
    std::vector<Triangle> m_tris;

    /**
     * @brief
     * When sorting the BVH nodes, the triangles are shifted around for better traversal later on.
     * However, swapping raw triangles around is slow, and it's harder to preserve GPU parity
     * with swapping the triangles around. Therefore, this indirection is used.
     */
    std::vector<int> m_triIndexes;
    std::vector<BVHNode> m_nodes;

    int m_planeCount;

    void UpdateNodeBounds(int _nodeIndex);

    float CalculateSAH(BVHNode& _node, int _axis, float _pos);

    float FindBestSplitPlane(BVHNode& _node, int& _bestAxis, float& _bestPos);

    float CalcNodeCost(BVHNode& _node);

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
    , m_dirtyIdxs(true)
    , m_dirtyTris(true)
    , m_planeCount(100)
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

    void PlaneCount(int _planes) { m_planeCount = _planes; };
    int PlaneCount() { return m_planeCount; };

    void BuildBHV();

    GLuint GetTriangleSSBO();

    GLuint GetIndexSSBO();

    GLuint GetNodeSSBO();
};

#endif