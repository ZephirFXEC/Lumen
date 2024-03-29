// Copyright (c) 2022.
// Enzo Crema
// All rights reserved

#define NOMINMAX

#include "Bvh.hpp"
#include <execution>
#include <functional>

#include <tbb/tbb.h>

namespace LumenRender {

constexpr int BINS = 12;

BVH::BVH(class IHittable<Mesh> *tri_mesh)
  : m_mesh(reinterpret_cast<Mesh *>(tri_mesh)),
    m_bvhNode(
      static_cast<BVHNode *>(_aligned_malloc(sizeof(BVHNode) * m_mesh->GetTriCount() * 2 + 64, sizeof(BVHNode)))),
    m_triIdx(static_cast<uint32_t *>(_aligned_malloc(sizeof(uint32_t) * m_mesh->GetTriCount(), sizeof(uint32_t))))
{
    Build();
}


void BVH::Build()
{

    m_nodeCount = 2;
    memset(m_bvhNode, 0, sizeof(BVHNode) * m_mesh->GetTriCount() * 2);

    // Populate Triangle indexes
    for (uint32_t i = 0; i < m_mesh->GetTriCount(); ++i) { m_triIdx[i] = i; }

    BVHNode &root = m_bvhNode[0];
    root.m_TriCount = static_cast<uint32_t>(m_mesh->GetTriCount());
    root.m_LeftFirst = 0;

    glm::vec3 centroid_min;
    glm::vec3 centroid_max;
    UpdateNodeBounds(0, centroid_min, centroid_max);

    m_buildStackPtr = 0;
    Subdivide(0, 0, m_nodeCount, centroid_min, centroid_max);

    std::array<uint32_t, 64> nodePtr{};
    uint32_t const N = m_buildStackPtr;
    nodePtr[0] = m_nodeCount;

    for (uint32_t i = 1; i < N; ++i) {
        nodePtr.at(i) = nodePtr.at(i - 1) + m_bvhNode[m_buildStack.at(i - 1).m_nodeidx].m_TriCount * 2;
    }

    for (uint32_t i = 0; i < N; ++i) {
        glm::vec3 cmin = m_buildStack.at(i).m_centroidMin;
        glm::vec3 cmax = m_buildStack.at(i).m_centroidMax;
        Subdivide(m_buildStack.at(i).m_nodeidx, 99, nodePtr.at(i), cmin, cmax);
    }
    m_nodeCount = static_cast<uint32_t>(m_mesh->GetTriCount()) * 2 + 64;

    FlattenBVH();
}


auto BVH::FindBestPlane(BVHNode &node, int &axis, int &splitPos, glm::vec3 &centroidMin, glm::vec3 &centroidMax) const
  -> float
{

    float bestCost = 1e30F;
    for (int a = 0; a < 3; ++a) {
        float const boundsMin = centroidMin[a];
        float const boundsMax = centroidMax[a];

        if (boundsMin == boundsMax) { continue; }

        // populate the bins
        std::array<Bin, BINS> bin{};
        float scale = BINS / (boundsMax - boundsMin);

        for (uint32_t i = 0; i < node.m_TriCount; ++i) {

            auto &triangle = m_mesh->GetTriangles()[m_triIdx[node.m_LeftFirst + i]];
            uint32_t const binIdx = std::min(static_cast<uint32_t>(BINS - 1),
              static_cast<uint32_t>((triangle->pData->Centroid[a] - boundsMin) * scale));
            bin[binIdx].m_TriCount++;
            AABB::Union(bin[binIdx].m_Bounds, triangle->mVertex[0]);
            AABB::Union(bin[binIdx].m_Bounds, triangle->mVertex[1]);
            AABB::Union(bin[binIdx].m_Bounds, triangle->mVertex[2]);
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

        for (uint32_t i = 0; i < BINS - 1; ++i) {
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
        for (uint32_t i = 0; i < BINS - 1; ++i) {
            float const planeCost = leftCount.at(i) * leftArea.at(i) + rightCount.at(i) * rightArea.at(i);
            if (planeCost < bestCost) {
                axis = a;
                splitPos = static_cast<int>(i + 1);
                bestCost = planeCost;
            }
        }
    }
    return bestCost;
}


void BVH::Subdivide(uint32_t nodeIdx, uint32_t depth, uint32_t &nodePtr, glm::vec3 &centroidMin, glm::vec3 &centroidMax)
{

    BVHNode &node = m_bvhNode[nodeIdx];

    // SAH
    int axis{};
    int splitPos{};
    float const splitCost = FindBestPlane(node, axis, splitPos, centroidMin, centroidMax);
    float const nosplitCost = BVHNode::CalculateNodeCost(node);
    if (splitCost >= nosplitCost) { return; }

    // END SAH

    uint32_t i = node.m_LeftFirst;
    uint32_t j = i + node.m_TriCount - 1;
    float const scale = BINS / (centroidMax[axis] - centroidMin[axis]);

    while (i <= j) {
        int const binIdx = std::min(BINS - 1,
          static_cast<int>((m_mesh->GetTriangles()[m_triIdx[i]]->pData->Centroid[axis] - centroidMin[axis]) * scale));
        if (binIdx < splitPos) {
            i++;
        } else {
            std::swap(m_triIdx[i], m_triIdx[j--]);
        }
    }

    uint32_t const leftCount = i - node.m_LeftFirst;
    if (leftCount == 0 || leftCount == node.m_TriCount) { return; }

    uint32_t const leftChildIdx = nodePtr++;
    uint32_t const rightChildIdx = nodePtr++;

    m_bvhNode[leftChildIdx].m_LeftFirst = node.m_LeftFirst;
    m_bvhNode[leftChildIdx].m_TriCount = leftCount;
    m_bvhNode[rightChildIdx].m_LeftFirst = i;
    m_bvhNode[rightChildIdx].m_TriCount = node.m_TriCount - leftCount;

    node.m_LeftFirst = leftChildIdx;
    node.m_TriCount = 0;

    UpdateNodeBounds(leftChildIdx, centroidMin, centroidMax);
    if (depth == 3) {
        // postpone the work, we'll do this in parallel later
        m_buildStack.at(m_buildStackPtr).m_nodeidx = leftChildIdx;
        m_buildStack.at(m_buildStackPtr).m_centroidMin = centroidMin;
        m_buildStack.at(m_buildStackPtr++).m_centroidMax = centroidMax;
    } else {
        Subdivide(leftChildIdx, depth + 1, nodePtr, centroidMin, centroidMax);
    }

    UpdateNodeBounds(rightChildIdx, centroidMin, centroidMax);
    if (depth == 3) {
        // postpone the work, we'll do this in parallel later
        m_buildStack.at(m_buildStackPtr).m_nodeidx = rightChildIdx;
        m_buildStack.at(m_buildStackPtr).m_centroidMin = centroidMin;
        m_buildStack.at(m_buildStackPtr++).m_centroidMax = centroidMax;
    } else {
        Subdivide(rightChildIdx, depth + 1, nodePtr, centroidMin, centroidMax);
    }
}


void BVH::UpdateNodeBounds(uint32_t nodeIdx, glm::vec3 &centroidMin, glm::vec3 &centroidMax) const
{
    BVHNode &node = m_bvhNode[nodeIdx];
    node.m_Bounds_min = glm::vec3(0.0F);
    node.m_Bounds_max = glm::vec3(0.0F);
    centroidMin = glm::vec3(0.0F);
    centroidMax = glm::vec3(0.0F);

    const uint32_t first = node.m_LeftFirst;

    for (uint32_t i = 0; i < node.m_TriCount; ++i) {
        const uint32_t leafTriIdx = m_triIdx[first + i];
        const auto leafTri = m_mesh->GetTriangles()[leafTriIdx];
        node.m_Bounds_min = glm::min(node.m_Bounds_min, leafTri->mVertex[0]);
        node.m_Bounds_min = glm::min(node.m_Bounds_min, leafTri->mVertex[1]);
        node.m_Bounds_min = glm::min(node.m_Bounds_min, leafTri->mVertex[2]);
        node.m_Bounds_max = glm::max(node.m_Bounds_max, leafTri->mVertex[0]);
        node.m_Bounds_max = glm::max(node.m_Bounds_max, leafTri->mVertex[1]);
        node.m_Bounds_max = glm::max(node.m_Bounds_max, leafTri->mVertex[2]);
        centroidMin = glm::min(centroidMin, leafTri->pData->Centroid);
        centroidMax = glm::max(centroidMax, leafTri->pData->Centroid);
    }
}

auto BVH::FlattenBVH() -> void
{
    m_FlattenBVH.resize(m_nodeCount);
    std::queue<uint32_t> queue;
    queue.push(0);
    uint32_t idx = 0;
    while (!queue.empty()) {
        uint32_t const nodeIdx = queue.front();
        queue.pop();
        m_FlattenBVH.at(idx++) = &m_bvhNode[nodeIdx];
        if (m_bvhNode[nodeIdx].m_TriCount == 0) {
            queue.push(m_bvhNode[nodeIdx].m_LeftFirst);
            queue.push(m_bvhNode[nodeIdx].m_LeftFirst + 1);
        }
    }
}


auto BVH::Traversal(const Ray &ray, float t_max) const -> bool
{
    if (!m_bvhNode) { return false; }

    // Precompute and store the inverse of ray.direction.
    glm::vec3 const inv_dir = glm::vec3(1.0F) / ray.mDirection;

    BVHNode *node = &m_bvhNode[0];
    std::array<BVHNode *, 64> stack{};
    uint32_t stackPtr = 0;

    bool hit = false;
    float closest = t_max;

    while (true) {

        if (node->isLeaf()) {

            for (uint32_t i = 0; i < node->m_TriCount; ++i) {

                const auto &leafTri = m_mesh->GetTriangles()[m_triIdx[node->m_LeftFirst + i]];

                // Use a early-exit strategy in the TriangleIntersect function.
                if (Triangle::TriangleIntersect(ray, *leafTri, m_triIdx[node->m_LeftFirst + i])
                    && ray.pRecord->mT < closest) {
                    hit = true;
                    closest = ray.pRecord->mT;
                    ray.pRecord->mNormal = leafTri->pData->N;
                }
            }


            if (stackPtr == 0) { break; }
            node = stack.at(--stackPtr);
            continue;
        }
        auto *left = &m_bvhNode[node->m_LeftFirst];
        auto *right = &m_bvhNode[node->m_LeftFirst + 1];

        // Use a faster intersection test for the bounding boxes.
        float t0 = AABB::IntersectAABB(ray, inv_dir, left->m_Bounds_min, left->m_Bounds_max);
        float t1 = AABB::IntersectAABB(ray, inv_dir, right->m_Bounds_min, right->m_Bounds_max);

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


bool BVH::Traversal_SSE(Ray &ray, float t_max) const { return true; }


bool BVH::Hit(const Ray &ray, float t_max) const { return Traversal(ray, t_max); }

AABB BVH::GetBounds(AABB &outbox) const
{
    outbox = AABB::Union(m_bvhNode[0].m_Bounds_min, m_bvhNode[0].m_Bounds_max);
    return outbox;
}


auto BVHNode::CalculateNodeCost(BVHNode &node) -> float
{
    glm::vec3 const e = node.m_Bounds_max - node.m_Bounds_min;// extent of the node
    float const surfaceArea = e.x * e.y + e.y * e.z + e.z * e.x;
    return surfaceArea * static_cast<float>(node.m_TriCount);
}


}// namespace LumenRender