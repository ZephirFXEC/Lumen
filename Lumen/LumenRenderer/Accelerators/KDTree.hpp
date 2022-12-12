// Copyright (c) 2022.
// Enzo Crema
// All rights reserved

//
// Created by enzoc on 08/12/2022.
//

#ifndef LUMEN_KDTREE_HPP
#define LUMEN_KDTREE_HPP

#include "../Utility/Utility.hpp"
#include "Bvh.hpp"
#include <smmintrin.h>
#include <xmmintrin.h>


namespace LumenRender {
class KDTree
{
  public:
    struct KDNode
    {
        union {
            struct
            {
                uint32_t left, right, parax;
                float splitPos;
            };// for an interior node
            struct
            {
                uint32_t first, count, dummy1, dummy2;
            };// for a leaf node, 16 bytes
        };
        union {
            __m128 bmin4;
            struct
            {
                glm::vec3 bmin;
                float w0;
            };
        };// 16 bytes
        union {
            __m128 bmax4;
            struct
            {
                glm::vec3 bmax;
                float w1;
            };
        };// 16 bytes
        union {
            __m128 minSize4;
            struct
            {
                glm::vec3 minSize;
                float w2;
            };
        };// 16 bytes, total: 64 bytes
        bool isLeaf() const { return (parax & 7) > 3; }
    };

    KDTree() = default;

    KDTree(TLASNode *tlasNodes, const uint32_t N, const uint32_t O)
      : tlas(tlasNodes), tlasCount(N), blasCount(N), offset(O)
    {
        if (leaf == nullptr) { leaf = new uint32_t[100000]; }
        node = (KDNode *)_aligned_malloc(sizeof(KDNode) * N * 2, 64);
        tlasIdx = new uint32_t[N * 2 + 64];
    }

    void Swap(const uint32_t a, const uint32_t b) const;

    void Rebuild();

    void RecurseRefit(uint32_t idx) const;

    void Subdivide(KDNode &node, uint32_t depth = 0);

    void Partition(KDNode &node, float splitPos, uint32_t axis);

    void Add(uint32_t idx);

    void RemoveLeaf(uint32_t idx);

    int FindNearest(uint32_t A, uint32_t &startB, float &startSA) const;


    // data
    KDNode *node = nullptr;
    TLASNode *tlas = nullptr;
    uint32_t *tlasIdx = nullptr, nodePtr = 1, tlasCount = 0, blasCount = 0, offset = 0, freed[2] = { 0, 0 };
    inline static uint32_t *leaf = nullptr;// will be shared between trees
};

}// namespace LumenRender
#endif// LUMEN_KDTREE_HPP