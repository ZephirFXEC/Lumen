//
// Created by enzoc on 16/10/2022.
//

#ifndef LUMEN_BVH_HPP
#define LUMEN_BVH_HPP

#include "../Scene/Object.hpp"
#include "../Structure/Mesh.hpp"
#include <vector>


namespace LumenRender {
    struct BVHNode {

        AABB m_Bounds;
        uint32_t m_TriCount{};
        uint32_t m_LeftFirst{};

        [[nodiscard]] auto isLeaf() const -> bool { return m_TriCount > 0; } // empty BVH leaves do not exist
        [[nodiscard]] auto CalculateNodeCost() const -> float {
            return m_Bounds.SurfaceArea();
        }
    };


    class BVH : public IHittable<BVH> {
    public:
        BVH() = default;

        explicit BVH(class Mesh* tri_mesh);

        void Build();

        void Subdivide(uint32_t nodeIdx);

        void UpdateNodeBounds(uint32_t nodeIdx) const;

        auto Traversal(Ray& ray, uint32_t nodeIdx, float t_max) const -> bool;

        auto Hit(Ray &ray, float t_max) const -> bool;

        auto GetBounds(AABB &outbox) const -> bool;

        [[nodiscard]] auto DeepCopy() const -> std::shared_ptr<IHittable>;


        Mesh* m_mesh{};
        BVHNode *m_bvhNode{};
        uint32_t *m_triIdx{}, m_rootNodeIdx{}, m_nodeCount{};
    };


} // LumenRender

#endif //LUMEN_BVH_HPP