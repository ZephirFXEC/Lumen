//
// Created by enzoc on 16/10/2022.
//

#ifndef LUMEN_BVH_HPP
#define LUMEN_BVH_HPP

#include "../Scene/Object.hpp"
#include "../Structure/Mesh.hpp"
#include <vector>


namespace LumenRender {

    struct Bin {
        AABB m_Bounds{};
        uint32_t m_TriCount{0};
    };


    struct BVHNode {

        union { struct { glm::vec3 m_Bounds_min; uint32_t m_LeftFirst; }; __m128 m_Bounds_min_m128; };
        union { struct { glm::vec3 m_Bounds_max; uint32_t m_TriCount;  }; __m128 m_Bounds_max_m128; };

        [[nodiscard]] auto isLeaf() const -> bool { return m_TriCount > 0; } // empty BVH leaves do not exist
    };


    class BVH : public IHittable<BVH> {
    public:
        BVH() = default;

        explicit BVH(class IHittable<Mesh>* tri_mesh);

        void Build();

        void Subdivide(uint32_t nodeIdx);

        void UpdateNodeBounds(uint32_t nodeIdx) const;

        auto Traversal(Ray& ray, uint32_t nodeIdx, float t_max) const -> bool;

        auto Hit(Ray &ray, float t_max) const -> bool;

        auto GetBounds(AABB &outbox) const -> AABB;

        auto CalculateSAH(BVHNode& node, int& axis, float& pos) const -> float;

        auto FindBestPlane(BVHNode& node, int& axis, float& splitPos) const -> float;

        static auto CalculateNodeCost(BVHNode& node) -> float;


        [[nodiscard]] auto DeepCopy() const -> std::shared_ptr<IHittable>;


        Mesh* m_mesh{};
        BVHNode *m_bvhNode{};
        uint32_t *m_triIdx{}, m_rootNodeIdx{}, m_nodeCount{};

    };


} // LumenRender

#endif //LUMEN_BVH_HPP