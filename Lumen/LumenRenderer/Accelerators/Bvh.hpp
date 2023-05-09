// Copyright (c) 2022.
// Enzo Crema
// All rights reserved

#ifndef LUMEN_BVH_HPP
#define LUMEN_BVH_HPP

#include "../Structure/Mesh.hpp"
#include "../Structure/Triangle.hpp"

#include <optional>
#include <vector>
#include <xmmintrin.h>


namespace LumenRender {

struct Bin
{
    AABB m_Bounds = {};
    uint32_t m_TriCount = 0;
};

struct BVHNode
{

    union {
        struct
        {
            glm::vec3 m_Bounds_min;
            uint32_t m_LeftFirst;
        };
        __m128 m_Bounds_min_m128;
    };
    union {
        struct
        {
            glm::vec3 m_Bounds_max;
            uint32_t m_TriCount;
        };
        __m128 m_Bounds_max_m128;
    };

    [[nodiscard]] constexpr auto isLeaf() const noexcept -> bool { return m_TriCount > 0; }
    // empty BVH leaves do not exist

    static auto CalculateNodeCost(BVHNode &node) -> float;
};

class BVH : public IHittable<BVH>
{

    struct BuildJob
    {
        uint32_t m_nodeidx;
        glm::vec3 m_centroidMin = {}, m_centroidMax = {};
    };

  public:
    BVH() = default;
    ~BVH() = default;

    explicit BVH(class IHittable<Mesh> *tri_mesh);

    void Build();

    bool Traversal(const Ray &ray, float t_max) const;

    bool Traversal_SSE(Ray &ray, float t_max) const;

    bool Hit(const Ray &ray, float t_max) const;

    AABB GetBounds(AABB &outbox) const;


  private:
    void Subdivide(uint32_t nodeIdx, uint32_t depth, uint32_t &nodePtr, glm::vec3 &centroidMin, glm::vec3 &centroidMax);

    void UpdateNodeBounds(uint32_t nodeIdx, glm::vec3 &centroidMin, glm::vec3 &centroidMax) const;

    float FindBestPlane(BVHNode &node, int &axis, int &splitPos, glm::vec3 &centroidMin, glm::vec3 &centroidMax) const;

    void FlattenBVH();

  public:
    Mesh *m_mesh = nullptr;
    BVHNode *m_bvhNode = nullptr;
    uint32_t *m_triIdx = nullptr, m_nodeCount = 0;
    std::array<BuildJob, 64> m_buildStack = {};
    uint32_t m_buildStackPtr = 0;

    std::vector<BVHNode *> m_FlattenBVH = {};
};
}// namespace LumenRender

#endif// LUMEN_BVH_HPP