//
// Created by enzoc on 16/10/2022.
//

#ifndef LUMEN_BVH_HPP
#define LUMEN_BVH_HPP

#include "../Scene/Object.hpp"
#include "../Scene/Scene.hpp"
#include "../Structure/Mesh.hpp"
#include <vector>

namespace LumenRender {

    struct BVHNode
    {
        union { struct { glm::vec3 aabbMin; uint32_t leftFirst; }; __m128 aabbMin4; };
        union { struct { glm::vec3 aabbMax; uint32_t triCount; }; __m128 aabbMax4; };
        bool isLeaf() const { return triCount > 0; } // empty BVH leaves do not exist
        float CalculateNodeCost() const
        {
            glm::vec3 e = aabbMax - aabbMin; // extent of the node
            return (e.x * e.y + e.y * e.z + e.z * e.x) * triCount;
        }
    };


    __declspec(align(64)) class BVH
    {
        struct BuildJob
        {
            uint32_t nodeIdx;
            AABB bounds;
        };

    public:
        BVH() = default;
        BVH( class Mesh* mesh );
        void Build();
        void Refit();
        void Intersect( Ray& ray, uint32_t instanceIdx );
    private:
        void Subdivide( uint32_t nodeIdx, uint32_t depth, uint32_t& nodePtr, glm::vec3& centroidMin, glm::vec3& centroidMax );
        void UpdateNodeBounds( uint32_t nodeIdx, glm::vec3& centroidMin, glm::vec3& centroidMax );
        float FindBestSplitPlane( BVHNode& node, int& axis, int& splitPos, glm::vec3& centroidMin, glm::vec3& centroidMax );
        class Mesh* mesh = nullptr;
    public:
        uint32_t* triIdx = nullptr;
        uint32_t nodesUsed{};
        BVHNode* bvhNode = nullptr;
        bool subdivToOnePrim = false; // for TLAS experiment
        BuildJob buildStack[64];
        int buildStackPtr{};
    };


} // LumenRender

#endif //LUMEN_BVH_HPP