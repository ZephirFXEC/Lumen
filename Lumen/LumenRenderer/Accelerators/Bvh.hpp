//
// Created by enzoc on 16/10/2022.
//

#ifndef LUMEN_BVH_HPP
#define LUMEN_BVH_HPP

#include "../Scene/Object.hpp"
#include "../Structure/Mesh.hpp"
#include <vector>


namespace LumenRender {


    __declspec(align(32)) struct BVHNode {

        AABB m_Bounds;
        uint32_t m_TriCount{};
        uint32_t m_LeftFirst{};

        bool isLeaf() const { return m_TriCount > 0; } // empty BVH leaves do not exist
        float CalculateNodeCost() const {
            return m_Bounds.SurfaceArea();
        }
    };


    class BVH : public Object {
    public:
        BVH() = default;

        explicit BVH(class Mesh *tri_mesh);

        void Build();

        void Subdivide(uint32_t nodeIdx);

        void UpdateNodeBounds(uint32_t nodeIdx) const;

        bool Traversal(Ray& ray, uint32_t nodeIdx, float t_max) const;

        bool Hit(Ray &ray, float t_max) const override;

        bool GetBounds(AABB &outbox) const override;

        [[nodiscard]] ObjectType GetType() const override { return ObjectType::BVH; }

    public:
        Mesh *m_mesh{};
        BVHNode *m_bvhNode{};
        uint32_t *m_triIdx{}, m_rootNodeIdx{}, m_nodeCount{};
    };


} // LumenRender

#endif //LUMEN_BVH_HPP