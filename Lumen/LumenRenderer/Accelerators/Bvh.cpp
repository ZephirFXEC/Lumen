//
// Created by enzoc on 16/10/2022.
//

#include <execution>
#include "Bvh.hpp"
#include "../Utility/Utility.hpp"


namespace LumenRender {

    void BVH::UpdateNodeBounds(uint32_t nodeIndex) {
        BVHNode& node = m_BVHNode.at(nodeIndex);
        node.aabb.pMin = glm::vec3( 1e30f );
        node.aabb.pMax = glm::vec3( -1e30f );
        for (uint32_t first = node.leftFirst, i = 0; i < node.triCount; i++)
        {
            uint32_t leafTriIdx = m_TriIdx[first + i];
            Triangle* leafTri = m_TriangleMesh[leafTriIdx];
            node.aabb.pMin = glm::min( node.aabb.pMin, leafTri->_v0 );
            node.aabb.pMin = glm::min( node.aabb.pMin, leafTri->_v1 );
            node.aabb.pMin = glm::min( node.aabb.pMin, leafTri->_v2 );
            node.aabb.pMax = glm::max( node.aabb.pMax, leafTri->_v0);
            node.aabb.pMax = glm::max( node.aabb.pMax, leafTri->_v1 );
            node.aabb.pMax = glm::max( node.aabb.pMax, leafTri->_v2 );
        }
    }



    void BVH::Subdivide(uint32_t nodeIndex) {
        // terminate recursion
        BVHNode& node = m_BVHNode.at(nodeIndex);
        if (node.triCount <= 2) return;
        // determine split axis and position
        glm::vec3 extent = node.aabb.pMax - node.aabb.pMin;
        int axis = 0;
        if (extent.y > extent.x) axis = 1;
        if (extent.z > extent[axis]) axis = 2;
        float splitPos = node.aabb.pMin[axis] + extent[axis] * 0.5f;
        // in-place partition
        uint32_t i = node.leftFirst;
        uint32_t j = i + node.triCount - 1;
        while (i <= j)
        {
            if (m_TriangleMesh[m_TriIdx[i]]->_centroid[axis] < splitPos)
                i++;
            else
                std::swap(m_TriIdx[i], m_TriIdx[j--] );
        }
        // abort split if one of the sides is empty
        uint32_t leftCount = i - node.leftFirst;
        if (leftCount == 0 || leftCount == node.triCount) return;

        // create child nodes
        uint32_t leftChildIdx = m_nodeUsed++;
        uint32_t rightChildIdx = m_nodeUsed++;

        m_BVHNode.at(leftChildIdx).leftFirst = node.leftFirst;
        m_BVHNode.at(leftChildIdx).triCount = leftCount;
        m_BVHNode.at(rightChildIdx).leftFirst = i;
        m_BVHNode.at(rightChildIdx).triCount = node.triCount - leftCount;

        node.leftFirst = leftChildIdx;
        node.triCount = 0;

        UpdateNodeBounds( leftChildIdx );
        UpdateNodeBounds( rightChildIdx );
        // recurse
        Subdivide( leftChildIdx );
        Subdivide( rightChildIdx );
    }



    void BVH::BuildBVH(const Scene& scene) {

        m_TriangleMesh = scene.m_Triangles;
        N = m_TriangleMesh.size();
        m_BVHNode.reserve(N * 2);
        m_TriIdx.reserve(N);

        for (uint32_t i = 0; i < N; i++) m_TriIdx[i] = i;

        BVHNode& root = m_BVHNode.at(m_rootIndex);
        root.leftFirst = 0, root.triCount = N;
        UpdateNodeBounds(m_rootIndex);

        Subdivide(m_rootIndex);

    }

    bool BVH::TraverseBVH(const Ray& ray, float tmax, const uint32_t nodeIndex, HitRecords& rec) const {

        bool hit = false;

        auto& node = m_BVHNode.at(nodeIndex);
        if (!node.aabb.Hit(ray, 0, INFINITY)) return hit;
        if (node.isLeaf())
        {
            for (uint32_t i = 0; i < node.triCount; i++ )
                if(m_TriangleMesh[m_TriIdx[node.leftFirst + i]]->Hit(ray, tmax, rec))
                    hit = true;
        }
        else
        {
            TraverseBVH( ray, tmax, node.leftFirst, rec);
            TraverseBVH( ray, tmax, node.leftFirst + 1, rec);
        }

        return hit;
    }

} // LumenRender