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
    GLuint m_triangleSSBOID;
    bool m_dirtyNodes;
    bool m_dirtyIDs;
    bool m_dirtyTris;

    struct Node
    {
        int leftNodeIndex;
        int firstTriangle;
        int triangleCount;

        glm::vec3 minBound;
        float _pad_1;
        glm::vec3 maxBound;
        float _pad_2;
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
        printf("Splitting node %i: ", _nodeIndex);
        Node& l_node = m_nodes[_nodeIndex];
        
        // Base case for recursion
        if (l_node.triangleCount <= 2)
        {
            printf("Only has %i tris\n", l_node.triangleCount);
            return;
        }

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
        int j = i + l_node.triangleCount - 1;
        while(i <= j)
        {
            if(CalculateCentroid(m_tris[m_triIndexes[i]])[axis] < l_splitPos)
            {
                i++;
            }
            else
            {
                j--;
                Triangle temp = m_tris[j];
                m_tris[j] = m_tris[i];
                m_tris[i] = temp;
            }
        }

        
        // Don't create empty sides
        int l_leftCount = i - l_node.firstTriangle;

        printf("%i left, %i right\n", l_leftCount, l_node.triangleCount - l_leftCount);

        if (l_leftCount == 0 || l_leftCount == l_node.triangleCount)
        {
            return;
        }

        // Create 2 split nodes
        int l_leftChildIndex = m_nodes.size();
        int l_rightChildIndex = m_nodes.size() + 1;

        printf("Creating 2 new nodes\n");
        
        Node l_left;
        l_left.firstTriangle = l_node.firstTriangle;
        l_left.triangleCount = l_leftCount;
        Node l_right;
        l_right.firstTriangle = i;
        l_right.triangleCount = l_node.triangleCount - l_leftCount;

        l_node.triangleCount = 0;
        l_node.leftNodeIndex = l_leftChildIndex;

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

    void BuildBHV()
    {
        printf("Building BVH with %i triangles\n", (int)m_tris.size());
        int rootNodeId = 0;
        for(int i = 0; i < m_tris.size(); i++)
        {
            CalculateCentroid(m_tris.at(i));
            m_triIndexes.push_back(i);
        }

        Node l_root;
        l_root.leftNodeIndex = 0;
        l_root.firstTriangle = 0;
        l_root.triangleCount = m_tris.size();
        m_nodes.push_back(l_root);

        UpdateNodeBounds(0);
        Subdivide(0);
        m_dirtyNodes = true;
        m_dirtyIDs = true;

        printf("BVH build with %i nodes\n", (int)m_nodes.size());

        for (int i = 0; i < m_nodes.size(); i++)
        {
            printf("Node %i: %i triangles, Min:(%f, %f, %f), Max(%f, %f, %f)\n", i, m_nodes[i].triangleCount,
                m_nodes[i].minBound[0], m_nodes[i].minBound[1], m_nodes[i].minBound[2],
                m_nodes[i].maxBound[0], m_nodes[i].maxBound[1], m_nodes[i].maxBound[2]);

            /*printf("%f,%f,%f,%f,%f,%f\n",
                m_nodes[i].minBound[0], m_nodes[i].minBound[1], m_nodes[i].minBound[2],
                m_nodes[i].maxBound[0], m_nodes[i].maxBound[1], m_nodes[i].maxBound[2]);*/
        }

        for (int i = 0; i < m_triIndexes.size(); i++)
        {
            if (i != m_triIndexes[i])
            {
                printf("Indexes: %i, %i\n", i, m_triIndexes[i]);
            }
        }
    }

    GLuint GetTriangleSSBO()
    {
        if (m_dirtyTris)
        {
            glGenBuffers(1, &m_triangleSSBOID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_triangleSSBOID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, (4 * sizeof(GLfloat) * 4) * m_tris.size(), &(m_tris.at(0)), GL_DYNAMIC_READ);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_triangleSSBOID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            printf("BVH triangles uploaded\n");
            m_dirtyTris = false;
        }

        return m_triangleSSBOID;
    }

    GLuint GetIndexSSBO()
    {
        if (m_dirtyIDs)
        {
            glGenBuffers(1, &m_indexesSSBOID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indexesSSBOID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint) * m_triIndexes.size(), &(m_triIndexes.at(0)), GL_DYNAMIC_READ);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_indexesSSBOID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            printf("BVH indexes uploaded\n");
            m_dirtyIDs = false;
        }

        return m_indexesSSBOID;
    }

    GLuint GetNodeSSBO()
    {
        if(m_dirtyNodes)
        {
            glGenBuffers(1, &m_nodeSSBOID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nodeSSBOID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, ((3 * sizeof(GLint)) + (4 * sizeof(GLfloat) * 2)) * m_nodes.size(), &(m_nodes.at(0)), GL_DYNAMIC_READ);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_nodeSSBOID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            printf("BVH Nodes uploaded\n");
            m_dirtyNodes = false;
        }

        return m_nodeSSBOID;
    }
};

#endif