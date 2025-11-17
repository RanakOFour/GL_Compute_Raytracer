#ifndef BVH_H
#define BVH_H

#include <vector>
#include <iostream>

#include "GL/glew.h"
#include "GLM/glm.hpp"

#include "Triangle.h"

struct BVH
{
    private:
    GLuint m_nodeSSBOID;
    GLuint m_indexesSSBOID;
    bool m_dirty;

    struct Node
    {
        glm::vec3 minBound;
        int leftNodeIndex; //Fits between the 2 vec3s to save on padding
        glm::vec3 maxBound;
        int rightNodeIndex;

        int firstTriangle;
        int triangleCount;
    };

    std::vector<Node> m_nodes;
    std::vector<Triangle> m_tris;
    std::vector<int> m_triIndexes;

    void UpdateNodeBounds(int _nodeIndex)
    {
        Node& l_nodeToUpdate = m_nodes[_nodeIndex];
        l_nodeToUpdate.minBound = glm::vec3(1e30f);
        l_nodeToUpdate.maxBound = glm::vec3(-1e30f);
        for(int first = l_nodeToUpdate.firstTriangle, i = 0; i < l_nodeToUpdate.triangleCount; i++)
        {
            int l_leafIndex = m_triIndexes[first + i];
            Triangle& leaf = m_tris[l_leafIndex];
            l_nodeToUpdate.minBound = glm::min(l_nodeToUpdate.minBound, leaf.a);
            l_nodeToUpdate.minBound = glm::min(l_nodeToUpdate.minBound, leaf.b);
            l_nodeToUpdate.minBound = glm::min(l_nodeToUpdate.minBound, leaf.c);

            l_nodeToUpdate.maxBound = glm::max(l_nodeToUpdate.maxBound, leaf.a);
            l_nodeToUpdate.maxBound = glm::max(l_nodeToUpdate.maxBound, leaf.b);
            l_nodeToUpdate.maxBound = glm::max(l_nodeToUpdate.maxBound, leaf.c);
        }
    }

    void Subdivide(int _nodeIndex)
    {
        Node& l_node = m_nodes[_nodeIndex];
        
        // Base case for recursion
        if(l_node.triangleCount <= 2) return;

        // Split axis
        glm::vec3 l_extents = l_node.maxBound - l_node.minBound;
        int axis = 0;
        if(l_extents.y > l_extents.x)
        {
            axis = 1;
        }
        if(l_extents.z > l_extents[axis])
        {
            axis = 2;
        }
        float l_splitPos = l_node.minBound[axis] + l_extents[axis] * 0.5f;

        //  In-place partition
        int i = l_node.firstTriangle;
        int j = l_node.triangleCount - 1;
        while(i <= j)
        {
            if(m_tris[i].centroid[axis] < l_splitPos)
            {
                i++;
            }
            else
            {
                Triangle temp = m_tris[j--];
                m_tris[j] = m_tris[i];
                m_tris[i] = temp;
            }
        }

        // Don't create empty sides
        int l_leftCount = i - l_node.firstTriangle;
        if(l_leftCount = 0 || l_leftCount == l_node.triangleCount) return;

        // Create 2 split nodes
        int l_leftChildIndex = m_nodes.size();
        int l_rightChildIndex = m_nodes.size() + 1;
        
        Node l_left;
        l_left.firstTriangle = l_node.firstTriangle;
        l_left.triangleCount = l_leftCount;
        Node l_right;
        l_right.firstTriangle = l_node.firstTriangle;
        l_right.triangleCount = l_node.triangleCount - l_leftCount;

        l_node.triangleCount = 0;
        l_node.leftNodeIndex = l_leftChildIndex;
        l_node.rightNodeIndex = l_rightChildIndex;

        m_nodes.push_back(l_left);
        m_nodes.push_back(l_right);

        UpdateNodeBounds(l_leftChildIndex);
        UpdateNodeBounds(l_rightChildIndex);

        Subdivide(l_leftChildIndex);
        Subdivide(l_rightChildIndex);
    };

    public:
    BVH(std::vector<Triangle>* _tris)
    : m_nodeSSBOID(0)
    , m_indexesSSBOID(0)
    , m_nodes()
    , m_tris(*_tris)
    , m_triIndexes()
    , m_dirty(true)
    {
        BuildBHV();
    };

    ~BVH()
    {

    };

    void BuildBHV()
    {
        int rootNodeId = 0;
        for(int i = 0; i < m_tris.size(); i++)
        {
            CalculateCentroid(m_tris.at(i));
            m_triIndexes.push_back(i);
        }

        Node l_root;
        l_root.leftNodeIndex = l_root.rightNodeIndex = 0;
        l_root.firstTriangle = 0;
        l_root.triangleCount = m_tris.size();
        m_nodes.push_back(l_root);

        UpdateNodeBounds(0);
        Subdivide(0);
        m_dirty = true;
    }

    GLuint GetNodeSSBO()
    {
        if(m_dirty)
        {
            glGenBuffers(1, &m_nodeSSBOID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nodeSSBOID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, ((2 * sizeof(GLfloat) * 4) + (2 * sizeof(GLint))) * m_nodes.size(), &(m_nodes.at(0)), GL_DYNAMIC_READ);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_nodeSSBOID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            printf("BVH Nodes uploaded\n");
            m_dirty = false;
        }

        return m_nodeSSBOID;
    }

    GLuint GetIndexSSBO()
    {
        if(m_dirty)
        {
            glGenBuffers(1, &m_indexesSSBOID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indexesSSBOID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, ((2 * sizeof(GLfloat) * 4) + (2 * sizeof(GLint))) * m_triIndexes.size(), &(m_triIndexes.at(0)), GL_DYNAMIC_READ);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_indexesSSBOID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            printf("BVH Nodes uploaded\n");
            m_dirty = false;
        }

        return m_indexesSSBOID;
    }
};

#endif