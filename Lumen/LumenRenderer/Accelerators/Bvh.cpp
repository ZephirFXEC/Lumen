//
// Created by enzoc on 16/10/2022.
//

#include <execution>
#include "Bvh.hpp"
#include "../Utility/Utility.hpp"


#define BINS 8

namespace LumenRender {

    BVH::BVH(struct Mesh *tri_mesh) {
        mesh = tri_mesh;
        bvhNode = (BVHNode *) _aligned_malloc(sizeof(BVHNode) * mesh->m_TriCount * 2 + 64, 64);
        triIdx = new uint32_t[mesh->m_TriCount];
        Build();
    }

    void BVH::Build() {

        // reset node pool
        nodesUsed = 2;
        memset(bvhNode, 0, mesh->m_TriCount * 2 * sizeof(BVHNode));
        // populate triangle index array
        for (int i = 0; i < mesh->m_TriCount; i++) triIdx[i] = i;
        // calculate triangle centroids for partitioning
        auto tri = mesh->m_Triangles;
        for (int i = 0; i < mesh->m_TriCount; i++)
            mesh->m_Triangles.at(i)->centroid =
                    (tri.at(i)->vertex0 + tri.at(i)->vertex1 + tri.at(i)->vertex2) * 0.3333f;
        // assign all triangles to root node
        BVHNode &root = bvhNode[0];
        root.leftFirst = 0, root.triCount = mesh->m_TriCount;
        AABB bounds;
        UpdateNodeBounds(0, bounds);
        // subdivide recursively
        buildStackPtr = 0;
        Subdivide(0, 0, nodesUsed, bounds);
        // do the parallel tasks, if any
        uint32_t nodePtr[64];
        int N = buildStackPtr;
        nodePtr[0] = nodesUsed;
        for (int i = 1; i < N; i++) nodePtr[i] = nodePtr[i - 1] + bvhNode[buildStack[i - 1].nodeIdx].triCount * 2;

//#pragma omp parallel for schedule(dynamic,1)
        for (int i = 0; i < N; i++) {
            AABB nbounds = { buildStack[i].bounds.pMin, buildStack[i].bounds.pMax };
            Subdivide(buildStack[i].nodeIdx, 99, nodePtr[i], nbounds);
        }
        nodesUsed = mesh->m_TriCount * 2 + 64;

    }

    void BVH::Subdivide(uint32_t nodeIdx, uint32_t depth, uint32_t &nodePtr, AABB &bounds) {

        BVHNode &node = bvhNode[nodeIdx];
        // determine split axis using SAH
        int axis, splitPos;
        float splitCost = FindBestSplitPlane(node, axis, splitPos, bounds);
        // terminate recursion
        if (subdivToOnePrim) {
            if (node.triCount == 1) return;
        } else {
            float nosplitCost = node.CalculateNodeCost();
            if (splitCost >= nosplitCost) return;
        }
        // in-place partition
        uint32_t i = node.leftFirst;
        uint32_t j = i + node.triCount - 1;
        float scale = BINS / (bounds.pMax[axis] - bounds.pMin[axis]);
        while (i <= j) {
            // use the exact calculation we used for binning to prevent rare inaccuracies
            int binIdx = std::min(BINS - 1,
                                  (int) ((mesh->m_Triangles[triIdx[i]]->centroid[axis] - bounds.pMin[axis]) * scale));

            if (binIdx < splitPos) {
                i++;
            } else {
                std::swap(triIdx[i], triIdx[j--]);
            }
        }
        // abort split if one of the sides is empty
        uint32_t leftCount = i - node.leftFirst;
        if (leftCount == 0 || leftCount == node.triCount) return; // never happens for dragon mesh, nice
        // create child nodes
        uint32_t leftChildIdx = nodePtr++;
        uint32_t rightChildIdx = nodePtr++;
        bvhNode[leftChildIdx].leftFirst = node.leftFirst;
        bvhNode[leftChildIdx].triCount = leftCount;
        bvhNode[rightChildIdx].leftFirst = i;
        bvhNode[rightChildIdx].triCount = node.triCount - leftCount;
        node.leftFirst = leftChildIdx;
        node.triCount = 0;
        // recurse
        UpdateNodeBounds(leftChildIdx, bounds);
        if (depth == 3) {
            // postpone the work, we'll do this in parallel later
            buildStack[buildStackPtr].nodeIdx = leftChildIdx;
            buildStack[buildStackPtr].bounds.pMin = bounds.pMin;
            buildStack[buildStackPtr++].bounds.pMax = bounds.pMax;
        } else Subdivide(leftChildIdx, depth + 1, nodePtr, bounds);

        UpdateNodeBounds(rightChildIdx, bounds);
        if (depth == 3) {
            // postpone the work, we'll do this in parallel later
            buildStack[buildStackPtr].nodeIdx = rightChildIdx;
            buildStack[buildStackPtr].bounds.pMin = bounds.pMin;
            buildStack[buildStackPtr++].bounds.pMax = bounds.pMax;
        } else Subdivide(rightChildIdx, depth + 1, nodePtr, bounds);

    }

    void BVH::UpdateNodeBounds(uint32_t nodeIdx, AABB &bounds) {

        BVHNode &node = bvhNode[nodeIdx];

        node.aabbMin = glm::vec3(1e30f);
        node.aabbMax = glm::vec3(-1e30f);
        bounds.pMin = glm::vec3(1e30f);
        bounds.pMax = glm::vec3(-1e30f);

        for (uint32_t first = node.leftFirst, i = 0; i < node.triCount; i++) {
            uint32_t leafTriIdx = triIdx[first + i];
            Triangle *leafTri = mesh->m_Triangles.at(leafTriIdx);
            node.aabbMin = glm::min(node.aabbMin, leafTri->vertex0);
            node.aabbMin = glm::min(node.aabbMin, leafTri->vertex1);
            node.aabbMin = glm::min(node.aabbMin, leafTri->vertex2);
            node.aabbMax = glm::max(node.aabbMax, leafTri->vertex0);
            node.aabbMax = glm::max(node.aabbMax, leafTri->vertex1);
            node.aabbMax = glm::max(node.aabbMax, leafTri->vertex2);
            bounds.pMin = glm::min(bounds.pMin, leafTri->centroid);
            bounds.pMax = glm::max(bounds.pMax, leafTri->centroid);
        }

    }

    float BVH::FindBestSplitPlane(BVHNode &node, int &axis, int &splitPos, AABB &bounds) {

        float bestCost = 1e30f;
        for (int a = 0; a < 3; a++) {
            float boundsMin = bounds.pMin[a], boundsMax = bounds.pMax[a];
            if (boundsMin == boundsMax) continue;
            // populate the bins
            float scale = BINS / (boundsMax - boundsMin);
            float leftCountArea[BINS - 1], rightCountArea[BINS - 1];
            int leftSum = 0, rightSum = 0;

            struct Bin { AABB bounds; int triCount = 0; } bin[BINS];

            for (uint32_t i = 0; i < node.triCount; i++) {
                auto triangle = mesh->m_Triangles.at(triIdx[node.leftFirst + i]);
                int binIdx = std::min(BINS - 1, (int) ((triangle->centroid[a] - boundsMin) * scale));
                bin[binIdx].triCount++;
                bin[binIdx].bounds = AABB::Union(bin[binIdx].bounds, triangle->vertex0);
                bin[binIdx].bounds = AABB::Union(bin[binIdx].bounds, triangle->vertex1);
                bin[binIdx].bounds = AABB::Union(bin[binIdx].bounds, triangle->vertex2);
            }
// gather data for the 7 planes between the 8 bins
            AABB leftBox, rightBox;
            for (int i = 0; i < BINS - 1; i++) {
                leftBox = AABB::Union(leftBox, bin[i].bounds);
                rightBox = AABB::Union(rightBox, bin[BINS - 1].bounds);
                leftSum += bin[i].triCount;
                rightSum += bin[BINS - 1].triCount;
                leftCountArea[i] = (float) leftSum * leftBox.SurfaceArea();
                rightCountArea[i] = (float) rightSum * rightBox.SurfaceArea();
            }
            // calculate SAH cost for the 7 planes
            scale = (boundsMax - boundsMin) / BINS;
            for (int i = 0; i < BINS - 1; i++) {
                const float planeCost = leftCountArea[i] + rightCountArea[i];
                if (planeCost < bestCost)
                    axis = a, splitPos = i + 1, bestCost = planeCost;
            }
        }
        return bestCost;
    }

    void BVH::Refit() {

        for (uint32_t i = nodesUsed - 1; i >= 0; i--) if (i != 1)
            {
                BVHNode& node = bvhNode[i];
                if (node.isLeaf())
                {
                    // leaf node: adjust bounds to contained triangles
                    AABB dummy; // we don't need centroid bounds here
                    UpdateNodeBounds(i, dummy);
                    continue;
                }
                // interior node: adjust bounds to child node bounds
                BVHNode& leftChild = bvhNode[node.leftFirst];
                BVHNode& rightChild = bvhNode[node.leftFirst + 1];
                node.aabbMin = glm::min( leftChild.aabbMin, rightChild.aabbMin );
                node.aabbMax = glm::max( leftChild.aabbMax, rightChild.aabbMax );
            }

    }

    void BVH::Intersect(Ray &ray, uint32_t instanceIdx) {

        BVHNode* node = &bvhNode[0], * stack[64];
        uint32_t stackPtr = 0;
        while (true)
        {
            if (node->isLeaf())
            {
                for (uint32_t i = 0; i < node->triCount; i++)
                {
                    uint32_t instPrim = (instanceIdx << 20) + triIdx[node->leftFirst + i];
                    if(mesh->m_Triangles.at(instPrim & 0xfffff )->Hit(ray, std::numeric_limits<float>::max())) {;
                        ray.m_Record.m_PrimIndex = instPrim;
                    }
                }
                if (stackPtr == 0) break; else node = stack[--stackPtr];
                continue;
            }
            BVHNode* child1 = &bvhNode[node->leftFirst];
            BVHNode* child2 = &bvhNode[node->leftFirst + 1];

            float dist1 = IntersectAABB( ray, child1->aabbMin, child1->aabbMax );
            float dist2 = IntersectAABB( ray, child2->aabbMin, child2->aabbMax );
            if (dist1 > dist2) { std::swap( dist1, dist2 ); std::swap( child1, child2 ); }
            if (dist1 == 1e30f)
            {
                if (stackPtr == 0) break; else node = stack[--stackPtr];
            }
            else
            {
                node = child1;
                if (dist2 != 1e30f) stack[stackPtr++] = child2;
            }
        }

    }

    float BVH::IntersectAABB(Ray &ray, glm::vec3 bmin, glm::vec3 bmax) {
        float tx1 = (bmin.x - ray.Origin.x) * ray.Direction.x, tx2 = (bmax.x - ray.Origin.x) * ray.Direction.x;
        float tmin = glm::min( tx1, tx2 ), tmax = glm::max( tx1, tx2 );
        float ty1 = (bmin.y - ray.Origin.y) * ray.Direction.y, ty2 = (bmax.y - ray.Origin.y) * ray.Direction.y;
        tmin = glm::max( tmin, glm::min( ty1, ty2 ) ), tmax = glm::min( tmax, glm::max( ty1, ty2 ) );
        float tz1 = (bmin.z - ray.Origin.z) * ray.Direction.z, tz2 = (bmax.z - ray.Origin.z) * ray.Direction.z;
        tmin = glm::max( tmin, glm::min( tz1, tz2 ) ), tmax = glm::min( tmax, glm::max( tz1, tz2 ) );
        if (tmax >= tmin && tmin < ray.m_Record.m_T && tmax > 0) return tmin; else return 1e30f;
    }
} // LumenRender