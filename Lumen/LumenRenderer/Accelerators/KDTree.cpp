// Copyright (c) 2022.
// Enzo Crema
// All rights reserved

//
// Created by enzoc on 08/12/2022.
//

#include "KDTree.hpp"

namespace LumenRender {

void KDTree::Swap(const uint32_t a, const uint32_t b) const
{
    uint32_t const t = tlasIdx[a];
    tlasIdx[a] = tlasIdx[b];
    tlasIdx[b] = t;
}


void KDTree::Rebuild()
{
    // we'll assume we get the same number of TLAS nodes each time
    tlasCount = blasCount;
    for (uint32_t i = 0; i < blasCount; i++) { tlasIdx[i] = i; }
    // subdivide root node
    node[0].first = 0;
    node[0].count = blasCount;
    node[0].parax = 7;

    nodePtr = 1;// root = 0, so node 1 is the first node we can create
    Subdivide(node[0]);// recursively subdivide the root node
    // "each node keeps it's cluster's minimum box sizes in each axis"
    for (int i = static_cast<int>(nodePtr - 1); i >= 0; i--) {
        if (node[i].isLeaf()) {
            node[i].minSize = glm::vec3(1e30F);
            for (uint32_t j = 0; j < node[i].count; j++) {
                uint32_t const idx = tlasIdx[node[i].first + j];
                leaf[idx + offset] = static_cast<uint32_t>(i);// we can find tlas[idx] in leaf node[i]
                glm::vec3 const tlSize = 0.5F * (tlas[idx].m_Bounds_max - tlas[idx].m_Bounds_min);
                node[i].minSize = glm::min(node[i].minSize, tlSize);
            }
        } else {
            node[i].minSize = glm::min(node[node[i].left].minSize, node[node[i].right].minSize);
        }
    }
}


void KDTree::RecurseRefit(uint32_t idx) const
{
    while (true) {
        if (idx == 0) { break; }

        idx = node[idx].parax >> 3;
        node[idx].minSize = glm::min(node[node[idx].left].minSize, node[node[idx].right].minSize);
        node[idx].bmin = glm::min(node[node[idx].left].bmin, node[node[idx].right].bmin);
        node[idx].bmax = glm::max(node[node[idx].left].bmax, node[node[idx].right].bmax);
    }
}


void KDTree::Subdivide(KDTree::KDNode &node, uint32_t depth)
{
    // update node bounds
    node.bmin = glm::vec3(1e30F);
    node.bmax = glm::vec3(-1e30F);
    node.minSize = glm::vec3(1e30F);
    for (uint32_t i = 0; i < node.count; i++) {
        TLASNode const &tln = tlas[tlasIdx[node.first + i]];
        glm::vec3 const C = (tln.m_Bounds_min + tln.m_Bounds_max) * 0.5F;
        node.minSize = glm::min(node.minSize, 0.5F * (tln.m_Bounds_max - tln.m_Bounds_min));
        node.bmin = glm::min(node.bmin, C);
        node.bmax = glm::max(node.bmax, C);
    }
    // terminate if we are down to 1 tlas
    if (node.count < 2) { return; }
    // claim left and right child nodes
    uint32_t const axis = DominantAxis(node.bmax - node.bmin);
    float center = (node.bmin[axis] + node.bmax[axis]) * 0.5F;
#if 1
    // try to balance (works quite well but doesn't seem to pay off)
    if (node.count > 150) {
        // count how many would go to the left
        int leftCount = 0;
        for (uint32_t i = 0; i < node.count; i++) {
            TLASNode const &tl = tlas[tlasIdx[node.first + i]];
            glm::vec3 P = (tl.m_Bounds_min + tl.m_Bounds_max) * 0.5F;
            if (P[axis] <= center) { leftCount++; }
        }
        float const ratio = glm::max(0.15F, glm::min(0.85F, (float)leftCount / (float)node.count));
        center = ratio * node.bmin[axis] + (1 - ratio) * node.bmax[axis];
    }
#endif
    Partition(node, center, axis);
    if (this->node[nodePtr].count == 0 || this->node[nodePtr + 1].count == 0) {
        return;// split failed
    }
    uint32_t const leftIdx = nodePtr;
    node.left = leftIdx, node.right = leftIdx + 1, nodePtr += 2;
    node.parax = (node.parax & 0xfffffff8) + axis, node.splitPos = center;
    Subdivide(this->node[leftIdx], depth + 1);
    Subdivide(this->node[leftIdx + 1], depth + 1);
}


void KDTree::Partition(KDTree::KDNode &node, float splitPos, uint32_t axis)
{
    uint32_t const N = node.count;
    uint32_t first = node.first;
    uint32_t last = first + N;
    if (N < 3) {
        last = first + 1;
    } else {
        while (true) {
            TLASNode const &tl = tlas[tlasIdx[first]];
            glm::vec3 P = (tl.m_Bounds_min + tl.m_Bounds_max) * 0.5F;
            if (P[axis] > splitPos) {
                Swap(first, --last);
            } else {
                first++;
            }
            if (first >= last) { break; }
        }
    }
    KDNode &left = this->node[nodePtr];
    KDNode &right = this->node[nodePtr + 1];
    left.first = node.first, right.first = last;
    left.count = right.first - left.first;
    left.parax = right.parax = (((uint32_t)(&node - this->node)) << 3) + 7;
    right.count = N - left.count;
}


void KDTree::Add(uint32_t idx)
{
    // capture bounds of new node
    idx -= offset;
    // create an index for the new node
    TLASNode const &newTLAS = tlas[idx];
    glm::vec3 const C = (newTLAS.m_Bounds_min + newTLAS.m_Bounds_max) * 0.5F;
    tlasIdx[tlasCount++] = idx;
    // claim a new KDNode for the tlas and make it a leaf
    uint32_t leafIdx{};
    uint32_t intIdx{};
    uint32_t nidx{};

    KDNode &leafNode = node[leafIdx = freed[0]];
    leaf[idx + offset] = leafIdx;
    leafNode.first = tlasCount - 1;
    leafNode.count = 1;
    leafNode.bmin = leafNode.bmax = C;
    leafNode.minSize = 0.5F * (newTLAS.m_Bounds_max - newTLAS.m_Bounds_min);
    // we'll also need a new interior node
    intIdx = freed[1];
    // see where we should insert it
    glm::vec3 const P = (newTLAS.m_Bounds_min + newTLAS.m_Bounds_max) * 0.5F;
    KDNode *n = &node[nidx = 0];
    while (true) {
        if (n->isLeaf()) {
            glm::vec3 Pn;
            if (nidx == 0)// special case: root is leaf ==> tree consists of only one node
            {
                node[intIdx] = node[0];// interior node slot is overwritten with old root (now sibling)
                node[intIdx].parax &= 7;// sibling's parent is the root node
                node[leafIdx].parax = 7;// new node's parent is the root node
                // 'split' the new KDNode over the greatest axis of separation
                Pn = (node[intIdx].bmin + node[intIdx].bmax) * 0.5F;
                // and finally, redirect leaf entries for old root
                for (uint32_t j = 0; j < node[intIdx].count; j++) {
                    leaf[tlasIdx[node[intIdx].first + j] + offset] = intIdx;
                }
                // put the new leaf and n in the correct fields
                nidx = intIdx;
                intIdx = 0;
                node[intIdx].parax = 0;
            } else {
                // replace one child of the parent by the new interior node
                KDNode &parent = node[n->parax >> 3];
                if (parent.left == nidx) {
                    parent.left = intIdx;
                } else {
                    parent.right = intIdx;
                }
                // rewire parent pointers
                node[intIdx].parax = n->parax & 0xfffffff8;
                n->parax = leafNode.parax = (intIdx << 3) + 7;
                // 'split' the new KDNode over the greatest axis of separation
                Pn = (n->bmin + n->bmax) * 0.5F;
            }
            // put the new leaf and n in the correct fields
            uint32_t const axis = DominantAxis(P - Pn);
            node[intIdx].parax += axis;
            node[intIdx].splitPos = ((Pn + P) * 0.5F)[axis];
            if (P[axis] < node[intIdx].splitPos) {
                node[intIdx].left = leafIdx, node[intIdx].right = nidx;
            } else {
                node[intIdx].right = leafIdx, node[intIdx].left = nidx;
            }
            break;
        } else {// traverse
            n = &node[nidx = ((P[n->parax & 7] < n->splitPos) ? n->left : n->right)];
        }
    }
    // refit
    RecurseRefit(leaf[idx + offset]);
}


void KDTree::RemoveLeaf(uint32_t idx)
{
    // determine which node to delete for tlas[idx]: must be a leaf
    uint32_t const toDelete = leaf[idx];
    idx -= offset;
    if (node[toDelete].count > 1)// special case: multiple TLASes in one node, rare
    {
        KDNode &n = node[toDelete];
        for (uint32_t j = 0; j < n.count; j++) {
            if (tlasIdx[n.first + j] == idx) tlasIdx[n.first + j] = tlasIdx[n.first + n.count-- - 1];
        }
        freed[0] = nodePtr++;
        freed[1] = nodePtr++;
        return;
    }
    uint32_t const parentIdx = node[toDelete].parax >> 3;
    KDNode &parent = node[parentIdx];
    uint32_t const sibling = parent.left == toDelete ? parent.right : parent.left;
    node[sibling].parax = (parent.parax & 0xfffffff8) + (node[sibling].parax & 7);
    parent = node[sibling];// by value, but rather elegant
    if (parent.isLeaf()) {// redirect leaf entries if the sibling is a leaf
        for (uint32_t j = 0; j < parent.count; j++) leaf[tlasIdx[parent.first + j] + offset] = parentIdx;
    } else {// make sure child nodes point to the new index
        node[parent.left].parax = (parentIdx << 3) + (node[parent.left].parax & 7);
        node[parent.right].parax = (parentIdx << 3) + (node[parent.right].parax & 7);
    }
    freed[0] = sibling;
    freed[1] = toDelete;
}


int KDTree::FindNearest(uint32_t A, uint32_t &startB, float &startSA) const
{
    // keep all hot data together
    A -= offset;
    __declspec(align(64)) struct TravState
    {
        __m128 Pa4, tlasAbmin4, tlasAbmax4;
        uint32_t n, stackPtr, bestB;
        float smallestSA;// exactly one cacheline
    } state{};
    uint32_t stack[60];
    uint32_t &n = state.n;
    uint32_t &stackPtr = state.stackPtr;
    uint32_t &bestB = state.bestB;
    float &smallestSA = state.smallestSA;
    n = 0;
    stackPtr = 0;
    smallestSA = startSA;
    bestB = startB - offset;
    // gather data for node A
    __m128 &tlasAbmin4 = state.tlasAbmin4;
    __m128 &tlasAbmax4 = state.tlasAbmax4;
    tlasAbmin4 = _mm_setr_ps(tlas[A].m_Bounds_min.x, tlas[A].m_Bounds_min.y, tlas[A].m_Bounds_min.z, 0);
    tlasAbmax4 = _mm_setr_ps(tlas[A].m_Bounds_max.x, tlas[A].m_Bounds_min.y, tlas[A].m_Bounds_min.z, 0);
    glm::vec3 const tlasAbmin = *(glm::vec3 *)&state.tlasAbmin4;
    glm::vec3 const tlasAbmax = *(glm::vec3 *)&state.tlasAbmax4;
    __m128 &Pa4 = state.Pa4;
    Pa4 = _mm_mul_ps(_mm_set_ps1(0.5F), _mm_add_ps(tlasAbmin4, tlasAbmax4));
    const __m128 half4 = _mm_set_ps1(0.5F);
    const __m128 extentA4 = _mm_sub_ps(tlasAbmax4, tlasAbmin4);
    const __m128 halfExtentA4 = _mm_mul_ps(half4, _mm_sub_ps(tlasAbmax4, tlasAbmin4));
    const __m128 tmp4 = _mm_setr_ps(-1, -1, -1, 1);
    const __m128 xyzMask4 = _mm_cmple_ps(tmp4, _mm_setzero_ps());

    // walk the tree
    while (true) {
        while (true) {
            if (node[n].isLeaf()) {
                // loop over the BLASes stored in this leaf
                for (uint32_t i = 0; i < node[n].count; i++) {
                    uint32_t const B = tlasIdx[node[n].first + i];
                    if (B == A) { continue; }
                    // calculate surface area of union of A and B
#if 0
						// scalar version
						const float3 size = fmaxf( tlasAbmax, tlas[B].aabbMax ) - fminf( tlasAbmin, tlas[B].aabbMin );
						const float SA = size.x * size.y + size.y * size.z + size.z * size.x;
#else
                    // SSE version
                    __m128 bbmin4 = _mm_and_ps(tlas[B].m_Bounds_min_m128, xyzMask4);
                    __m128 bbmax4 = _mm_and_ps(tlas[B].m_Bounds_max_m128, xyzMask4);
                    __m128 size4 = _mm_sub_ps(_mm_max_ps(tlasAbmax4, bbmax4), _mm_min_ps(tlasAbmin4, bbmin4));
                    float SA = size4.m128_f32[0] * size4.m128_f32[1] + size4.m128_f32[1] * size4.m128_f32[2]
                               + size4.m128_f32[2] * size4.m128_f32[0];
#endif
                    if (SA < smallestSA) {
                        smallestSA = SA;
                        bestB = B;
                    }
                }
                break;
            }
            // consider recursing into branches, sorted by distance
            uint32_t t;
            uint32_t nearNode = node[n].left;
            uint32_t farNode = node[n].right;
            if (Pa4.m128_f32[node[n].parax & 7] > node[n].splitPos) { t = nearNode, nearNode = farNode, farNode = t; }
            __m128 v0a = _mm_max_ps(_mm_sub_ps(node[nearNode].bmin4, Pa4), _mm_sub_ps(Pa4, node[nearNode].bmax4));
            __m128 v0b = _mm_max_ps(_mm_sub_ps(node[farNode].bmin4, Pa4), _mm_sub_ps(Pa4, node[farNode].bmax4));
            __m128 d4a = _mm_max_ps(extentA4, _mm_sub_ps(v0a, _mm_add_ps(node[nearNode].minSize4, halfExtentA4)));
            __m128 d4b = _mm_max_ps(extentA4, _mm_sub_ps(v0b, _mm_add_ps(node[farNode].minSize4, halfExtentA4)));
            float sa1 =
              d4a.m128_f32[0] * d4a.m128_f32[1] + d4a.m128_f32[1] * d4a.m128_f32[2] + d4a.m128_f32[2] * d4a.m128_f32[0];
            float sa2 =
              d4b.m128_f32[0] * d4b.m128_f32[1] + d4b.m128_f32[1] * d4b.m128_f32[2] + d4b.m128_f32[2] * d4b.m128_f32[0];
            float diff1 = sa1 - smallestSA;
            float diff2 = sa2 - smallestSA;
            const uint32_t visit = (diff1 < 0) * 2 + (diff2 < 0);
            if (visit == 0u) break;
            if (visit == 3) {
                stack[stackPtr++] = farNode;
                n = nearNode;
            } else if (visit == 2)
                n = nearNode;
            else {
                n = farNode;
            }
        }
        if (stackPtr == 0) { break; }
        n = stack[--stackPtr];
    }
    // all done; return best match
    startB = bestB + offset;
    startSA = smallestSA;
    return bestB + offset;
}
}// namespace LumenRender