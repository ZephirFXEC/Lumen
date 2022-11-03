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
        union { struct { glm::vec3 aabbMin; uint32_t leftFirst; }; __m128 aabbMin4; };
        union { struct { glm::vec3 aabbMax; uint32_t triCount; }; __m128 aabbMax4; };

        [[nodiscard]] bool isLeaf() const { return triCount > 0; } // empty BVH leaves do not exist
        [[nodiscard]] float CalculateNodeCost() const {
            glm::vec3 e = aabbMax - aabbMin; // extent of the node
            return (e.x * e.y + e.y * e.z + e.z * e.x) * (float) triCount;
        }
    };


    __declspec(align(64)) class BVH : public Object {

        struct BuildJob {
            uint32_t nodeIdx;
            AABB bounds;
        };

    public:
        BVH() = default;
        explicit BVH(Mesh *tri_mesh);

        void Build();

        void Refit();

        bool Hit(Ray &ray, float t_max) const override;

        bool GetBounds(AABB &outbox) const override;

        [[nodiscard]] ObjectType GetType() const override { return ObjectType::BVH; }

    private:
        void Subdivide(uint32_t nodeIdx, uint32_t depth, uint32_t &nodePtr, AABB &bounds);

        void UpdateNodeBounds(uint32_t nodeIdx, AABB &bounds);

        float FindBestSplitPlane(BVHNode &node, int &axis, int &splitPos, AABB &bounds);

        static float IntersectAABB(Ray &ray, glm::vec3 vec1, glm::vec3 vec2);

        class Mesh *mesh = nullptr;

    public:
        uint32_t *triIdx = nullptr;
        uint32_t nodesUsed{};
        BVHNode *bvhNode = nullptr;
        bool subdivToOnePrim = false; // for TLAS experiment
        BuildJob buildStack[64];
        int buildStackPtr{};

    };


} // LumenRender

#endif //LUMEN_BVH_HPP