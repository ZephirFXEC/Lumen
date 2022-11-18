//
// Created by enzoc on 16/10/2022.
//

#include <execution>
#include "Bvh.hpp"
#include "../Utility/Utility.hpp"


namespace LumenRender {

    BVH::BVH(class IHittable<Mesh>* tri_mesh) :
            m_mesh(dynamic_cast<Mesh*>(tri_mesh)),
            m_bvhNode(new BVHNode[static_cast<uint64_t>(m_mesh->m_TriCount * 2)]),
            m_triIdx(new uint32_t[m_mesh->m_TriCount]),
            m_nodeCount(1) {
        Build();
    }


    void BVH::Build() {

        //Populate Triangle indexes
        for (uint32_t i = 0; i < m_mesh->m_TriCount; i++) {
                m_triIdx[i] = i;
        }

        //Compute centroids
        for (uint32_t i = 0; i < m_mesh->m_TriCount; i++) {
            m_mesh->m_TriData[i].Centroid = (
                    m_mesh->m_Triangles[i].vertex[0] +
                    m_mesh->m_Triangles[i].vertex[1] +
                    m_mesh->m_Triangles[i].vertex[2]) / 3.0F;
        }

        BVHNode &root = m_bvhNode[m_rootNodeIdx];
        root.m_TriCount = m_mesh->m_TriCount;
        root.m_LeftFirst = 0;

        UpdateNodeBounds(m_rootNodeIdx);
        Subdivide(m_rootNodeIdx);
    }

    void BVH::Subdivide(uint32_t nodeIdx) {

        BVHNode &node = m_bvhNode[nodeIdx];
        if (node.m_TriCount <= 2) {
            {
                return;
            }
        }

        glm::vec3 extent = node.m_Bounds.pMax - node.m_Bounds.pMin;

        uint8_t axis = 0;
        if (extent.y > extent.x) {
            {
                axis = 1;
            }
        }
        if (extent.z > extent[axis]) {
            {
                axis = 2;
            }
        }

        float const splitPos = node.m_Bounds.pMin[axis] + extent[axis] * 0.5F;

        uint32_t i = node.m_LeftFirst;
        uint32_t j = i + node.m_TriCount - 1;

        while (i <= j) {
            if (m_mesh->m_TriData.at(m_triIdx[i]).Centroid[axis] < splitPos) {
                {
                    i++;
                }
            } else {
                {
                    std::swap(m_triIdx[i], m_triIdx[j--]);
                }
            }
        }

        uint32_t const leftCount = i - node.m_LeftFirst;
        if (leftCount == 0 || leftCount == node.m_TriCount) {
            {
                return;
            }
        }

        uint32_t const leftChildIdx = m_nodeCount++;
        uint32_t const rightChildIdx = m_nodeCount++;

        m_bvhNode[leftChildIdx].m_LeftFirst = node.m_LeftFirst;
        m_bvhNode[leftChildIdx].m_TriCount = leftCount;
        m_bvhNode[rightChildIdx].m_LeftFirst = i;
        m_bvhNode[rightChildIdx].m_TriCount = node.m_TriCount - leftCount;

        node.m_LeftFirst = leftChildIdx;
        node.m_TriCount = 0;

        UpdateNodeBounds(leftChildIdx);
        UpdateNodeBounds(rightChildIdx);

        Subdivide(leftChildIdx);
        Subdivide(rightChildIdx);
    }


    void BVH::UpdateNodeBounds(uint32_t nodeIdx) const {
        BVHNode &node = m_bvhNode[nodeIdx];
        node.m_Bounds = AABB();

        for (uint32_t i = 0; i < node.m_TriCount; i++) {
            uint32_t const leafTriIdx = m_triIdx[node.m_LeftFirst + i];
            auto leafTri = m_mesh->m_Triangles.at(leafTriIdx);
                node.m_Bounds = AABB::Union(node.m_Bounds, leafTri.vertex[0]);
                node.m_Bounds = AABB::Union(node.m_Bounds, leafTri.vertex[1]);
                node.m_Bounds = AABB::Union(node.m_Bounds, leafTri.vertex[2]);
        }
    }


    auto BVH::Traversal(Ray &ray, uint32_t nodeIdx, float t_max) const -> bool {
        Ray temp = ray;
        BVHNode const &node = m_bvhNode[nodeIdx];
        bool hit = false;
        float closest = t_max;

        if (!AABB::IntersectAABB(ray, node.m_Bounds)) {
            {
                return false;
            }
        }

        if (node.isLeaf()) {
            for (uint32_t i = 0; i < node.m_TriCount; i++) {

                uint32_t const idx = m_triIdx[node.m_LeftFirst + i];
                auto leafTri = m_mesh->m_Triangles.at(idx);

                if (LumenRender::Triangle::TriangleIntersect(temp, leafTri, idx) &&  temp.m_Record.m_T < closest) {
                    {
                        hit = true;
                        closest = temp.m_Record.m_T;
                        temp.m_Record.m_Normal = m_mesh->m_TriData.at(idx).N;
                        ray = temp;
                    }
                }
            }
            return hit;
        }

        hit |= Traversal(ray, node.m_LeftFirst, t_max);
        hit |= Traversal(ray, node.m_LeftFirst + 1, t_max);
        return hit;
    }


    auto BVH::Hit(Ray &ray, float t_max) const -> bool {
        return Traversal(ray, m_rootNodeIdx, t_max);
    }

    auto BVH::GetBounds(AABB &outbox) const -> bool {
        outbox = m_bvhNode[m_rootNodeIdx].m_Bounds;
        return true;
    }

    auto BVH::DeepCopy() const -> std::shared_ptr<IHittable> {
        return std::make_shared<BVH>(*this);
    }


} // LumenRender