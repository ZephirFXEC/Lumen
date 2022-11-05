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

        bool isLeaf() const { return m_TriCount > 0; } // empty BVH leaves do not exist
        float CalculateNodeCost() const {
            glm::vec3 e = m_Bounds.pMax - m_Bounds.pMin; // extent of the node
            return 2.0f * (e.x * e.y + e.x * e.z + e.y * e.z); // surface area of the node
        }
    };


    class BVH : public Object {


    public:
        BVH() = default;
        explicit BVH(Mesh *tri_mesh);

        void Build();

        void Traversal();

        bool Hit(Ray &ray, float t_max) const override;

        bool GetBounds(AABB &outbox) const override;

        [[nodiscard]] ObjectType GetType() const override { return ObjectType::BVH; }


    };


} // LumenRender

#endif //LUMEN_BVH_HPP