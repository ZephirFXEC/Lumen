//
// Created by enzoc on 16/10/2022.
//

#include <execution>
#include "Bvh.hpp"

#include <cmath>
#include "../Utility/Utility.hpp"


namespace LumenRender {

#define BINS 8


    BVH::BVH(class IHittable<Mesh> *tri_mesh) :
            m_mesh(dynamic_cast<Mesh *>(tri_mesh)),
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
            m_mesh->m_Triangles[i].Centroid = (
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


    auto BVH::CalculateSAH(BVHNode &node, int &axis, float &pos) const -> float {

        AABB leftBox = AABB();
        AABB rightBox = AABB();

        uint32_t leftCount = 0;
        uint32_t rightCount = 0;

        for (uint32_t i = 0; i < node.m_TriCount; i++) {
            auto &tri = m_mesh->m_Triangles[m_triIdx[node.m_LeftFirst + i]];
            if (tri.Centroid[axis] < pos) {
                leftCount++;
                leftBox = AABB::Union(leftBox, tri.vertex[0]);
                leftBox = AABB::Union(leftBox, tri.vertex[1]);
                leftBox = AABB::Union(leftBox, tri.vertex[2]);

            } else {
                rightCount++;
                rightBox = AABB::Union(rightBox, tri.vertex[0]);
                rightBox = AABB::Union(rightBox, tri.vertex[1]);
                rightBox = AABB::Union(rightBox, tri.vertex[2]);
            }
        }
        float const cost = leftCount * leftBox.SurfaceArea() + rightCount * rightBox.SurfaceArea();
        return cost > 0 ? cost : 1e30F;
    }


    auto BVH::FindBestPlane(BVHNode &node, int &axis, float &splitPos) const -> float {
        float bestCost = 1e30F;
        for (int a = 0; a < 3; a++) {
            float boundsMin = 0.F;
            float boundsMax = 0.F;
            for (uint32_t i = 0; i < node.m_TriCount; i++) {
                auto &triangle = m_mesh->m_Triangles[m_triIdx[node.m_LeftFirst + i]];
                boundsMin = std::min(boundsMin, triangle.Centroid[a]);
                boundsMax = std::max(boundsMax, triangle.Centroid[a]);
            }
            if (boundsMin == boundsMax) {
                continue;
            }

            // populate the bins
            std::array<Bin, BINS> bin{};
            float scale = BINS / (boundsMax - boundsMin);
            for (uint32_t i = 0; i < node.m_TriCount; i++) {

                auto &triangle = m_mesh->m_Triangles[m_triIdx[node.m_LeftFirst + i]];
                uint32_t const binIdx = std::min(static_cast<uint32_t>(BINS - 1),
                                                 static_cast<uint32_t>((triangle.Centroid[a] - boundsMin) * scale));
                bin.at(binIdx).m_TriCount++;
                AABB::Union(bin.at(binIdx).m_Bounds, triangle.vertex[0]);
                AABB::Union(bin.at(binIdx).m_Bounds, triangle.vertex[1]);
                AABB::Union(bin.at(binIdx).m_Bounds, triangle.vertex[2]);


            }
            // gather data for the 7 planes between the 8 bins
            std::array<float, BINS - 1> leftArea{};
            std::array<float, BINS - 1> rightArea{};
            std::array<uint32_t, BINS - 1> leftCount{};
            std::array<uint32_t, BINS - 1> rightCount{};

            AABB leftBox = AABB();
            AABB rightBox = AABB();
            uint32_t leftSum = 0;
            uint32_t rightSum = 0;

            for (uint32_t i = 0; i < BINS - 1; i++) {
                leftSum += bin.at(i).m_TriCount;
                leftCount.at(i) = leftSum;
                leftBox = AABB::Union(leftBox, bin.at(i).m_Bounds);
                leftArea.at(i) = leftBox.SurfaceArea();

                rightSum += bin.at(BINS - 1 - i).m_TriCount;
                rightCount.at(BINS - 2 - i) = rightSum;
                rightBox = AABB::Union(rightBox, bin.at(BINS - 1 - i).m_Bounds);
                rightArea.at(BINS - 2 - i) = rightBox.SurfaceArea();
            }

            // calculate SAH cost for the 7 planes
            scale = (boundsMax - boundsMin) / BINS;
            for (uint32_t i = 0; i < BINS - 1; i++) {
                float const planeCost = leftCount.at(i) * leftArea.at(i) + rightCount.at(i) * rightArea.at(i);
                if (planeCost < bestCost) {
                    axis = a;
                    splitPos = boundsMin + scale * (i + 1.F);
                    bestCost = planeCost;
                }
            }
        }
        return bestCost;
    }


    void BVH::Subdivide(uint32_t nodeIdx) {

        BVHNode &node = m_bvhNode[nodeIdx];

        // SAH
        int axis = 0;
        float splitPos = NAN;
        float const splitCost = FindBestPlane(node, axis, splitPos);
        float const nosplitCost = CalculateNodeCost(node);
        if (splitCost >= nosplitCost) {
            return;
        }

        // END SAH

        uint32_t i = node.m_LeftFirst;
        uint32_t j = i + node.m_TriCount - 1;

        while (i <= j) {
            if (m_mesh->m_Triangles[m_triIdx[i]].Centroid[axis] < splitPos) {
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

            node.m_Bounds_min = glm::min(node.m_Bounds_min, leafTri.vertex[0]);
            node.m_Bounds_min = glm::min(node.m_Bounds_min, leafTri.vertex[1]);
            node.m_Bounds_min = glm::min(node.m_Bounds_min, leafTri.vertex[2]);

            node.m_Bounds_max = glm::max(node.m_Bounds_max, leafTri.vertex[0]);
            node.m_Bounds_max = glm::max(node.m_Bounds_max, leafTri.vertex[1]);
            node.m_Bounds_max = glm::max(node.m_Bounds_max, leafTri.vertex[2]);
        }
    }


    auto BVH::Traversal(Ray &ray, uint32_t nodeIdx, float t_max) const -> bool {
        Ray temp = ray;
        BVHNode *node = &m_bvhNode[m_rootNodeIdx];
        std::array<BVHNode *, 64> stack{};
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
                node = stack.at(--stackPtr);
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
                node = stack.at(--stackPtr);
            } else {
                node = left;
                if (t1 != 1e30F) { stack.at(stackPtr++) = right; }
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

    auto BVH::CalculateNodeCost(BVHNode &node) -> float {
        glm::vec3 const e = node.m_Bounds_max - node.m_Bounds_min; // extent of the node
        float const surfaceArea = e.x * e.y + e.y * e.z + e.z * e.x;
        return node.m_TriCount * surfaceArea;
    }


} // LumenRender