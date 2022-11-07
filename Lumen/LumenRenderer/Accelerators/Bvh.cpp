//
// Created by enzoc on 16/10/2022.
//

#include <execution>
#include "Bvh.hpp"
#include "../Utility/Utility.hpp"


#define BINS 8

namespace LumenRender {

    BVH::BVH(class Mesh *tri_mesh) {
        m_mesh = tri_mesh;
        //m_bvhNode = (BVHNode*)_aligned_malloc( sizeof( BVHNode ) * m_mesh->m_TriCount * 2, 32 );
        m_bvhNode = new BVHNode[m_mesh->m_TriCount * 2];
        m_triIdx = new uint32_t[m_mesh->m_TriCount];
        m_rootNodeIdx = 0;
        m_nodeCount = 1;

        Build();
    }


    void BVH::Build() {

        //Populate Triangle indexes
        for(uint32_t i = 0; i<m_mesh->m_TriCount; i++) m_triIdx[i] = i;

        BVHNode& root = m_bvhNode[m_rootNodeIdx];
        root.m_TriCount = m_mesh->m_TriCount;
        root.m_LeftFirst = 0;

        UpdateNodeBounds(m_rootNodeIdx);
        Subdivide(m_rootNodeIdx);
    }

    void BVH::Subdivide(uint32_t nodeIdx)
    {

        BVHNode& node = m_bvhNode[nodeIdx];
        if (node.m_TriCount <= 2) return;

        glm::vec3 extent = node.m_Bounds.pMax - node.m_Bounds.pMin;

        uint8_t axis = 0;
        if (extent.y > extent.x) axis = 1;
        if (extent.z > extent[axis]) axis = 2;

        float splitPos = node.m_Bounds.pMin[axis] + extent[axis] * 0.5f;

        uint32_t i = node.m_LeftFirst;
        uint32_t j = i + node.m_TriCount - 1;

        while (i <= j)
        {
            if (m_mesh->m_Triangles.at(m_triIdx[i])->centroid[axis] < splitPos)
                i++;
            else
                std::swap(m_triIdx[i], m_triIdx[j--]);
        }

        uint32_t leftCount = i - node.m_LeftFirst;
        if (leftCount == 0 || leftCount == node.m_TriCount) return;

        uint32_t leftChildIdx = m_nodeCount++;
        uint32_t rightChildIdx = m_nodeCount++;

        m_bvhNode[leftChildIdx].m_LeftFirst = node.m_LeftFirst;
        m_bvhNode[leftChildIdx].m_TriCount = leftCount;
        m_bvhNode[rightChildIdx].m_LeftFirst = i;
        m_bvhNode[rightChildIdx].m_TriCount = node.m_TriCount - leftCount;

        node.m_LeftFirst = leftChildIdx;
        node.m_TriCount = 0;

        UpdateNodeBounds( leftChildIdx );
        UpdateNodeBounds( rightChildIdx );

        Subdivide( leftChildIdx );
        Subdivide( rightChildIdx );
    }


    void BVH::UpdateNodeBounds(uint32_t nodeIdx) const {
        BVHNode& node = m_bvhNode[nodeIdx];
        node.m_Bounds = AABB(glm::vec3(1e30), glm::vec3(-1e30));

        for (uint32_t first = node.m_LeftFirst, i = 0; i < node.m_TriCount; i++)
        {
            uint32_t leafTriIdx = m_triIdx[first + i];
            auto leafTri = m_mesh->m_Triangles.at(leafTriIdx);
            node.m_Bounds = AABB::Union(node.m_Bounds, leafTri->vertex0);
            node.m_Bounds = AABB::Union(node.m_Bounds, leafTri->vertex1);
            node.m_Bounds = AABB::Union(node.m_Bounds, leafTri->vertex2);
        }
    }


    bool BVH::Traversal(Ray& ray, uint32_t nodeIdx, float t_max) const {
        BVHNode& node = m_bvhNode[nodeIdx];
        bool hit = false;

        if (!AABB::IntersectAABB(ray, node.m_Bounds)) return hit;

        if (node.isLeaf())
        {
            for (uint32_t i = 0; i < node.m_TriCount; i++ )
            {
                auto tri = m_mesh->m_Triangles.at(m_triIdx[node.m_LeftFirst + i]);
                if(tri->Hit(ray, t_max))
                {
                    hit = true;
                }
            }
        } else {
            Traversal( ray, node.m_LeftFirst, t_max );
            Traversal( ray, node.m_LeftFirst + 1, t_max );
        }

        return hit;
    }


    bool BVH::Hit(Ray &ray, float t_max) const {
        Ray temp = ray;
        float closest = t_max;
        bool hit = false;

        if(Traversal(temp, m_rootNodeIdx, closest) && temp.m_Record.m_T < closest)
        {
            ray = temp;
            closest = ray.m_Record.m_T;
            hit = true;
        }

        return hit;
    }

    bool BVH::GetBounds(AABB &outbox) const {
        outbox = m_bvhNode[m_rootNodeIdx].m_Bounds;
        return true;
    }



} // LumenRender