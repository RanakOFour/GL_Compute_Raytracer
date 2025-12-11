#include "BVH.h"

void BVH::BuildBHV(std::vector<Triangle>* _tris)
{
    m_tris = _tris;
    m_nodes = std::vector<BVHNode>();
    m_triIndexes = std::vector<int>();

    printf("Building BVH with %i triangles\n", (int)_tris->size());
    int rootNodeId = 0;
    for(int i = 0; i < _tris->size(); i++)
    {
        CalculateCentroid(_tris->at(i));
        m_triIndexes.push_back(i);
    }

    BVHNode l_root;
    l_root.leftFirst = 0;
    l_root.triangleCount = _tris->size();
    m_nodes.push_back(l_root);

    UpdateNodeBounds(0);
    Subdivide(0);
    m_dirtyNodes = true;
    m_dirtyIdxs = true;

    printf("BVH build with %i nodes\n", (int)m_nodes.size());

    // for (int i = 0; i < m_nodes.size(); i++)
    // {
    //     printf("BVHNode %i: %i triangles, Min:(%f, %f, %f), Max(%f, %f, %f)\n", i, m_nodes[i].triangleCount,
    //         m_nodes[i].minBound[0], m_nodes[i].minBound[1], m_nodes[i].minBound[2],
    //         m_nodes[i].maxBound[0], m_nodes[i].maxBound[1], m_nodes[i].maxBound[2]);

    //     /*printf("%f,%f,%f,%f,%f,%f\n",
    //         m_nodes[i].minBound[0], m_nodes[i].minBound[1], m_nodes[i].minBound[2],
    //         m_nodes[i].maxBound[0], m_nodes[i].maxBound[1], m_nodes[i].maxBound[2]);*/
    // }
};

void BVH::UpdateNodeBounds(int _nodeIndex)
{
    BVHNode& l_nodeToUpdate = m_nodes[_nodeIndex];
    l_nodeToUpdate.minBound = glm::vec3(1e30f);
    l_nodeToUpdate.maxBound = glm::vec3(-1e30f);
    for(int i = 0; i < l_nodeToUpdate.triangleCount; i++)
    {
        int l_leafIndex = m_triIndexes[l_nodeToUpdate.leftFirst + i];
        Triangle& leaf = m_tris->at(l_leafIndex);
        l_nodeToUpdate.minBound = glm::min(l_nodeToUpdate.minBound, leaf.a);
        l_nodeToUpdate.minBound = glm::min(l_nodeToUpdate.minBound, leaf.b);
        l_nodeToUpdate.minBound = glm::min(l_nodeToUpdate.minBound, leaf.c);

        l_nodeToUpdate.maxBound = glm::max(l_nodeToUpdate.maxBound, leaf.a);
        l_nodeToUpdate.maxBound = glm::max(l_nodeToUpdate.maxBound, leaf.b);
        l_nodeToUpdate.maxBound = glm::max(l_nodeToUpdate.maxBound, leaf.c);
    }
}

float BVH::CalculateSAH(BVHNode& _node, int _axis, float _pos)
{
    AABB leftBox, rightBox;
    int leftCount = 0, rightCount = 0;
    for(int i = 0; i < _node.triangleCount; i++)
    {
        Triangle& tri = m_tris->at(m_triIndexes[_node.leftFirst + i]);
        if(CalculateCentroid(tri)[_axis] < _pos)
        {
            leftCount++;
            leftBox.Grow(tri.a);
            leftBox.Grow(tri.b);
            leftBox.Grow(tri.c);
        }
        else
        {
            rightCount++;
            rightBox.Grow(tri.a);
            rightBox.Grow(tri.b);
            rightBox.Grow(tri.c);
        }
    }
    
    float cost = leftCount * leftBox.Area() + rightCount * rightBox.Area();
    return cost > 0 ? cost : 1e30f;
}

float BVH::FindBestSplitPlane(BVHNode& _node, int& _bestAxis, float& _bestPos)
{
    float l_bestCost = 1e30f;

    for (int axis = 0; axis < 3; axis++)
    {
        float l_minBound = _node.minBound[axis];
        float l_maxBound = _node.maxBound[axis];

        if (l_minBound == l_maxBound)
        {
            continue;
        }

        float l_scale = (l_maxBound - l_minBound) / (float)m_planeCount;

        for (int i = 1; i < m_planeCount; i++)
        {
            float l_candidatePos = l_minBound + i * l_scale;
            float l_cost = CalculateSAH(_node, axis, l_candidatePos);
            if (l_cost < l_bestCost)
            {
                l_bestCost = l_cost;
                _bestPos = l_candidatePos;
                _bestAxis = axis;
            }
        }
    }

    return l_bestCost;
};

float BVH::CalcNodeCost(BVHNode& _node)
{
    glm::vec3 l_extents = _node.maxBound - _node.minBound;
    float l_surfaceArea = l_extents.x * l_extents.y + l_extents.y * l_extents.z + l_extents.z * l_extents.x;
    return _node.triangleCount * l_surfaceArea;
};

void BVH::Subdivide(int _nodeIndex)
{
    //printf("Splitting node %i: ", _nodeIndex);
    BVHNode& l_node = m_nodes[_nodeIndex];
    
    // Base case for recursion
    if (l_node.triangleCount <= 2)
    {
        //printf("Only has %i tris\n", l_node.triangleCount);
        return;
    }

    // Use SAH for split axis
    int l_bestAxis = -1;
    float l_bestPos, l_bestCost = 1e30f;
    float l_splitCost = FindBestSplitPlane(l_node, l_bestAxis, l_bestPos);

    //  In-place partition
    int i = l_node.leftFirst;
    int j = i + l_node.triangleCount - 1;
    while(i <= j)
    {
        if(CalculateCentroid(m_tris->at(m_triIndexes[i]))[l_bestAxis] < l_bestPos)
        {
            i++;
        }
        else
        {
            int temp = m_triIndexes[i];
            m_triIndexes[i] = m_triIndexes[j];
            m_triIndexes[j]= temp;
            j--;
        }
    }

    
    // Don't create empty sides
    int l_leftCount = i - l_node.leftFirst;

    //printf("%i left, %i right\n", l_leftCount, l_node.triangleCount - l_leftCount);

    float l_nodeCost = CalcNodeCost(l_node);

    if(l_splitCost >= l_nodeCost)
    {
        return;
    }

    // Create 2 split nodes
    int l_leftChildIndex = m_nodes.size();
    int l_rightChildIndex = m_nodes.size() + 1;

    //printf("Creating 2 new nodes\n");
    
    BVHNode l_left;
    l_left.leftFirst = l_node.leftFirst;
    l_left.triangleCount = l_leftCount;
    BVHNode l_right;
    l_right.leftFirst = i;
    l_right.triangleCount = l_node.triangleCount - l_leftCount;

    l_node.triangleCount = 0;
    l_node.leftFirst = l_leftChildIndex;

    m_nodes.push_back(l_left);
    m_nodes.push_back(l_right);

    UpdateNodeBounds(l_leftChildIndex);
    UpdateNodeBounds(l_rightChildIndex);

    Subdivide(l_leftChildIndex);
    Subdivide(l_rightChildIndex);
};

GLuint BVH::GetIndexSSBO()
{
    if (m_dirtyIdxs)
    {
        glGenBuffers(1, &m_indexesSSBOID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indexesSSBOID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint) * m_triIndexes.size(), &(m_triIndexes.at(0)), GL_DYNAMIC_READ);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        printf("BVH indexes uploaded\n");
        m_dirtyIdxs = false;
    }

    return m_indexesSSBOID;
}

GLuint BVH::GetNodeSSBO()
{
    if(m_dirtyNodes)
    {
        glGenBuffers(1, &m_nodeSSBOID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nodeSSBOID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(BVHNode) * m_nodes.size(), &(m_nodes.at(0)), GL_DYNAMIC_READ);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        printf("BVH Nodes uploaded\n");
        m_dirtyNodes = false;
    }

    return m_nodeSSBOID;
}