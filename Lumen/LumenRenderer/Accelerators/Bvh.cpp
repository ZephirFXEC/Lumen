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



    auto BVH::CalculateSAH(BVHNode& node, int axis, float pos) const -> float {
        AABB leftBox, rightBox = AABB();
        int leftCount = 0, rightCount = 0;
        for (uint32_t i = 0; i < node.m_TriCount; i++)
        {
            auto& triCentroid = m_mesh->m_TriData[m_triIdx[node.m_LeftFirst + i]];
            auto& tri = m_mesh->m_Triangles[m_triIdx[node.m_LeftFirst + i]];
            if (triCentroid.Centroid[axis] < pos)
            {
                leftCount++;
                leftBox = AABB::Union(leftBox, tri.vertex[0]);
                leftBox = AABB::Union(leftBox, tri.vertex[1]);
                leftBox = AABB::Union(leftBox, tri.vertex[2]);

            }
            else
            {
                rightCount++;
                rightBox = AABB::Union(rightBox, tri.vertex[0]);
                rightBox = AABB::Union(rightBox, tri.vertex[1]);
                rightBox = AABB::Union(rightBox, tri.vertex[2]);
            }
        }
        float const cost = leftCount * leftBox.SurfaceArea() + rightCount * rightBox.SurfaceArea();
        return cost > 0 ? cost : 1e30F;
    }



    void BVH::Subdivide(uint32_t nodeIdx) {

        BVHNode &node = m_bvhNode[nodeIdx];

        // SAH
        int bestAxis = -1;
        float bestPos = 0;
        float bestCost = 1e30F;

        for (int axis = 0; axis < 3; axis++) {
            for (uint32_t i = 0; i < node.m_TriCount; i++) {
                auto &triangle = m_mesh->m_TriData[m_triIdx[node.m_LeftFirst + i]];
                float const candidatePos = triangle.Centroid[axis];
                float const cost = CalculateSAH(node, axis, candidatePos);
                if (cost < bestCost) {
                    bestPos = candidatePos;
                    bestAxis = axis;
                    bestCost = cost;
                }
            }
        }

        int const axis = bestAxis;
        float const splitPos = bestPos;
        glm::vec3 e = node.m_Bounds_max - node.m_Bounds_min; // extent of parent
        float parentArea = e.x * e.y + e.y * e.z + e.z * e.x;
        float const parentCost = node.m_TriCount * parentArea;
        if (bestCost >= parentCost) {
            return;
        }

        // END SAH

        uint32_t i = node.m_LeftFirst;
        uint32_t j = i + node.m_TriCount - 1;

        while (i <= j) {
            if (m_mesh->m_TriData[m_triIdx[i]].Centroid[axis] < splitPos) {
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
        node.m_Bounds_min = glm::vec3(0.0F);
        node.m_Bounds_max = glm::vec3(0.0F);

        for (uint32_t i = 0; i < node.m_TriCount; i++) {
            uint32_t const leafTriIdx = m_triIdx[node.m_LeftFirst + i];
            auto leafTri = m_mesh->m_Triangles[leafTriIdx];

            node.m_Bounds_min = glm::min( node.m_Bounds_min, leafTri.vertex[0] );
            node.m_Bounds_min = glm::min( node.m_Bounds_min, leafTri.vertex[1] );
            node.m_Bounds_min = glm::min( node.m_Bounds_min, leafTri.vertex[2] );

            node.m_Bounds_max = glm::max( node.m_Bounds_max, leafTri.vertex[0] );
            node.m_Bounds_max = glm::max( node.m_Bounds_max, leafTri.vertex[1] );
            node.m_Bounds_max = glm::max( node.m_Bounds_max, leafTri.vertex[2] );
        }
    }


    auto BVH::Traversal(Ray &ray, uint32_t nodeIdx, float t_max) const -> bool {
        Ray temp = ray;
        BVHNode *node = &m_bvhNode[m_rootNodeIdx];
        std::array<BVHNode*, 64> stack{};
        uint32_t stackPtr = 0;

        bool hit = false;
        float closest = t_max;

        while (true) {
            if (node->isLeaf()) {
                for (uint32_t i = 0; i < node->m_TriCount; ++i) {

                    uint32_t const leafTriIdx = m_triIdx[node->m_LeftFirst + i];
                    auto leafTri = m_mesh->m_Triangles[leafTriIdx];

                    if (LumenRender::Triangle::TriangleIntersect(temp, leafTri, leafTriIdx) &&
                        temp.m_Record.m_T < closest) {
                        hit = true;
                        closest = temp.m_Record.m_T;
                        temp.m_Record.m_Normal = m_mesh->m_TriData[leafTriIdx].N;
                        ray = temp;
                    }
                }

                if (stackPtr == 0) { break; }
                node = stack[--stackPtr];
                continue;

            }
            BVHNode *left = &m_bvhNode[node->m_LeftFirst];
            BVHNode *right = &m_bvhNode[node->m_LeftFirst + 1];

#if 1
            float t0 = LumenRender::AABB::IntersectAABB(temp, left->m_Bounds_min, left->m_Bounds_max);
            float t1 = LumenRender::AABB::IntersectAABB(temp, right->m_Bounds_min, right->m_Bounds_max);
#else
            float t0 = LumenRender::AABB::IntersectAABB_SSE(temp, left->m_Bounds_min_m128, left->m_Bounds_max_m128);
            float t1 = LumenRender::AABB::IntersectAABB_SSE(temp, right->m_Bounds_min_m128, right->m_Bounds_max_m128);
#endif
            if (t0 > t1) {
                std::swap(t0, t1);
                std::swap(left, right);
            }
            if (t0 == 1e30F) {
                if (stackPtr == 0) { break; }
                node = stack[--stackPtr];
            } else {
                node = left;
                if (t1 != 1e30F) { stack[stackPtr++] = right; }
            }
        }
        return hit;
    }

    auto BVH::Hit(Ray &ray, float t_max) const -> bool {
        return Traversal(ray, m_rootNodeIdx, t_max);
    }

    auto BVH::GetBounds(AABB &outbox) const -> AABB {
        outbox = AABB::Union(m_bvhNode[m_rootNodeIdx].m_Bounds_min,
                             m_bvhNode[m_rootNodeIdx].m_Bounds_max);
        return outbox;
    }

    auto BVH::DeepCopy() const -> std::shared_ptr<IHittable> {
        return std::make_shared<BVH>(*this);
    }


} // LumenRender