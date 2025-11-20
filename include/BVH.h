#ifndef BVH_H
#define BVH_H

#include <vector>
#include <iostream>
#include <algorithm>

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
        int _pad;

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

    struct aabb
    {
        glm::vec3 bmin;
        glm::vec3 bmax;

        aabb() :
        bmin(1e30f),
        bmax(-1e30f)
        {

        }

        inline void grow(glm::vec3& _p)
        {
            bmin = glm::min(bmin, _p);
            bmax = glm::max(bmax, _p);
        };

        inline float area()
        {
            glm::vec3 e = bmax - bmin;
            return e.x * e.y + e.y * e.z + e.z * e.x;
        };
    };

    float CalculateSAH(Node& _node, int _axis, float _pos)
    {
        aabb leftBox, rightBox;
        int leftCount = 0, rightCount = 0;
        for(int i = 0; i < _node.triangleCount; i++)
        {
            Triangle& tri = m_tris[m_triIndexes[_node.firstTriangle + i]];
            if(CalculateCentroid(tri)[_axis] < _pos)
            {
                leftCount++;
                leftBox.grow(tri.a);
                leftBox.grow(tri.b);
                leftBox.grow(tri.c);
            }
            else
            {
                rightCount++;
                rightBox.grow(tri.a);
                rightBox.grow(tri.b);
                rightBox.grow(tri.c);
            }
        }
        
        float cost = leftCount * leftBox.area() + rightCount * rightBox.area();
        return cost > 0 ? cost : 1e30f;
    }

    void Subdivide(int _nodeIndex)
    {
        //printf("Splitting node %i: ", _nodeIndex);
        Node& l_node = m_nodes[_nodeIndex];
        
        // Base case for recursion
        if (l_node.triangleCount <= 2)
        {
            //printf("Only has %i tris\n", l_node.triangleCount);
            return;
        }

        // Use SAH for split axis
        int bestAxis = -1;
        float bestPos, bestCost = 1e30f;
        for(int axis = 0; axis < 3; axis++)
        {
            for(int i = 0; i < l_node.triangleCount; i++)
            {
                Triangle& tri = m_tris[m_triIndexes[l_node.firstTriangle + i]];
                float candidatePos = CalculateCentroid(tri)[axis];
                float cost = CalculateSAH(l_node, axis, candidatePos);
                if(cost < bestCost)
                {
                    bestPos = candidatePos;
                    bestAxis = axis;
                    bestCost = cost;
                }
            }
        }

        //  In-place partition
        int i = l_node.firstTriangle;
        int j = i + l_node.triangleCount - 1;
        while(i <= j)
        {
            if(CalculateCentroid(m_tris[m_triIndexes[i]])[bestAxis] < bestPos)
            {
                i++;
            }
            else
            {
                // j--;
                // int temp = m_triIndexes[j];
                // m_triIndexes[j] = m_triIndexes[i];
                // m_triIndexes[i]= temp;

                std::swap(m_triIndexes[i], m_triIndexes[j--]);
            }
        }

        
        // Don't create empty sides
        int l_leftCount = i - l_node.firstTriangle;

        //printf("%i left, %i right\n", l_leftCount, l_node.triangleCount - l_leftCount);

        glm::vec3 l_e = l_node.maxBound - l_node.minBound;
        float l_parentArea = l_e.x * l_e.y + l_e.y * l_e.z + l_e.z * l_e.x;
        float l_parentCost = l_node.triangleCount *l_parentArea;

        if(bestCost >= l_parentCost)
        {
            return;
        }

        // Create 2 split nodes
        int l_leftChildIndex = m_nodes.size();
        int l_rightChildIndex = m_nodes.size() + 1;

        //printf("Creating 2 new nodes\n");
        
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

        // for (int i = 0; i < m_nodes.size(); i++)
        // {
        //     printf("Node %i: %i triangles, Min:(%f, %f, %f), Max(%f, %f, %f)\n", i, m_nodes[i].triangleCount,
        //         m_nodes[i].minBound[0], m_nodes[i].minBound[1], m_nodes[i].minBound[2],
        //         m_nodes[i].maxBound[0], m_nodes[i].maxBound[1], m_nodes[i].maxBound[2]);

        //     /*printf("%f,%f,%f,%f,%f,%f\n",
        //         m_nodes[i].minBound[0], m_nodes[i].minBound[1], m_nodes[i].minBound[2],
        //         m_nodes[i].maxBound[0], m_nodes[i].maxBound[1], m_nodes[i].maxBound[2]);*/
        // }
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
            glBufferData(GL_SHADER_STORAGE_BUFFER, ((4 * sizeof(GLint)) + (4 * sizeof(GLfloat) * 2)) * m_nodes.size(), &(m_nodes.at(0)), GL_DYNAMIC_READ);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_nodeSSBOID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            printf("BVH Nodes uploaded\n");
            m_dirtyNodes = false;
        }

        return m_nodeSSBOID;
    }
};

#endif