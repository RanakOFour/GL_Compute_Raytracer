/**
 * @file BVH.h
 * @brief Bounding Volume Hierarchy for ray intersection acceleration
 * 
 * This file provides the BVH class which constructs and manages a tree
 * structure that groups triangles for accelerated ray-triangle intersection
 * testing in the raytracer.
 */

#ifndef BVH_H
#define BVH_H

#include <vector>
#include <iostream>

#include "GL/glew.h"
#include "GLM/glm.hpp"

#include "Datastructs/Triangle.h"
#include "Datastructs/AABB.h"

/**
 * @class BVH
 * @brief Bounding Volume Hierarchy for accelerating ray-tracing processes
 * 
 * A tree structure that groups triangles into bounding volumes to reduce
 * the number of ray-triangle intersection tests needed during raytracing.
 * Uses Surface Area Heuristic (SAH) for optimal tree construction.
 */
class BVH
{
private:
    /** @brief OpenGL SSBO ID for BVH nodes */
    GLuint m_nodeSSBOID;
    
    /** @brief OpenGL SSBO ID for triangle indices */
    GLuint m_indexesSSBOID;

    /**
     * @name Dirty flags
     * @brief Flags indicating when data should be re-uploaded to the GPU
     * @{
     */
    /** @brief True if node data needs to be re-uploaded */
    bool m_dirtyNodes;
    
    /** @brief True if index data needs to be re-uploaded */
    bool m_dirtyIdxs;
    /** @} */

    /**
     * @struct BVHNode
     * @brief Internal node structure for the BVH tree
     * 
     * Aligned for GPU compatibility with 16-byte alignment.
     */
    struct BVHNode
    {
        glm::vec3 minBound;   ///< Minimum bound of the node's AABB
        int leftFirst;        ///< For leaves: first triangle index. For internal: left child index
        glm::vec3 maxBound;   ///< Maximum bound of the node's AABB
        int triangleCount;    ///< Number of triangles (0 for internal nodes)
    };

    /**
     * @brief Temporary reference to the triangle list used for BVH construction
     * 
     * Only stored to avoid passing std::vector<Triangle>* to every function.
     */
    std::vector<Triangle>* m_tris;

    /**
     * @brief Indirection array for triangle access
     * 
     * When sorting BVH nodes, triangles are shifted around for better traversal.
     * This indirection array is used instead of swapping raw triangles for
     * performance and GPU parity preservation.
     */
    std::vector<int> m_triIndexes;
    
    /** @brief Array of BVH nodes forming the tree structure */
    std::vector<BVHNode> m_nodes;

    /** @brief Number of planes to test when finding optimal split */
    int m_planeCount;

    /**
     * @brief Update the bounds of a node based on its contained triangles
     * @param _nodeIndex Index of the node to update
     */
    void UpdateNodeBounds(int _nodeIndex);

    /**
     * @brief Calculate the Surface Area Heuristic cost for a split
     * @param _node Reference to the node being evaluated
     * @param _axis Axis to split on (0=X, 1=Y, 2=Z)
     * @param _pos Position along the axis to split at
     * @return SAH cost for this split configuration
     */
    float CalculateSAH(BVHNode& _node, int _axis, float _pos);

    /**
     * @brief Find the optimal split plane using SAH
     * @param _node Reference to the node to split
     * @param _bestAxis Output parameter for the best split axis
     * @param _bestPos Output parameter for the best split position
     * @return The SAH cost of the best split found
     */
    float FindBestSplitPlane(BVHNode& _node, int& _bestAxis, float& _bestPos);

    /**
     * @brief Calculate the traversal cost for a node
     * @param _node Reference to the node
     * @return Estimated traversal cost
     */
    float CalcNodeCost(BVHNode& _node);

    /**
     * @brief Recursively subdivide a node into child nodes
     * @param _nodeIndex Index of the node to subdivide
     */
    void Subdivide(int _nodeIndex);

public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty BVH ready for building.
     */
    BVH()
    : m_nodeSSBOID(0)
    , m_indexesSSBOID(0)
    , m_nodes()
    , m_tris()
    , m_triIndexes()
    , m_dirtyNodes(true)
    , m_dirtyIdxs(true)
    , m_planeCount(100)
    {
    };

    /**
     * @brief Constructor that immediately builds a BVH from triangles
     * @param _tris Pointer to the vector of triangles to accelerate
     */
    BVH(std::vector<Triangle>* _tris)
    : m_nodeSSBOID(0)
    , m_indexesSSBOID(0)
    , m_nodes()
    , m_tris(_tris)
    , m_triIndexes()
    , m_dirtyNodes(true)
    , m_dirtyIdxs(true)
    , m_planeCount(100)
    {
        BuildBHV(_tris);
    };

    /**
     * @brief Destructor - releases OpenGL resources
     */
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
    };

    /**
     * @brief Set the number of planes to test when finding splits
     * @param _planes Number of split planes to test per axis
     */
    void PlaneCount(int _planes) { m_planeCount = _planes; };
    
    /**
     * @brief Get the current plane count setting
     * @return Number of planes tested per axis
     */
    int PlaneCount() { return m_planeCount; };

    /**
     * @brief Build the BVH from a triangle list
     * @param _tris Pointer to the vector of triangles
     */
    void BuildBHV(std::vector<Triangle>* _tris);

    /**
     * @brief Get the OpenGL SSBO containing triangle indices
     * @return SSBO ID for triangle indices
     */
    GLuint GetIndexSSBO();

    /**
     * @brief Get the OpenGL SSBO containing BVH nodes
     * @return SSBO ID for BVH nodes
     */
    GLuint GetNodeSSBO();
};

#endif