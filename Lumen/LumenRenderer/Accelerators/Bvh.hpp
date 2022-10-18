//
// Created by enzoc on 16/10/2022.
//

#ifndef LUMEN_BVH_HPP
#define LUMEN_BVH_HPP

#include "../Scene/Object.hpp"
#include "../Scene/Scene.hpp"
#include "../Structure/Triangle_Mesh.hpp"
#include <vector>

namespace LumenRender {

    struct BVHNode {
        AABB aabb;
        uint32_t leftFirst{}, triCount{};
        bool isLeaf() const { return triCount > 0; }
    };

    class BVH {
    public:
        BVH() = default;
        void BuildBVH(const Scene& scene);
        void UpdateNodeBounds(uint32_t nodeIndex);
        void Subdivide(uint32_t nodeIndex);
        bool TraverseBVH(const Ray& ray, float tmax, uint32_t nodeIndex, HitRecords& rec) const;

    public:
        std::vector<Triangle*> m_TriangleMesh;
        std::vector<BVHNode> m_BVHNode;
        std::vector<uint32_t> m_TriIdx{};
        uint32_t m_rootIndex{}, m_nodeUsed{}, N{};

    };



} // LumenRender

#endif //LUMEN_BVH_HPP