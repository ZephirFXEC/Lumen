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
      AABB m_Bounds{ AABB() };
      uint32_t m_TriCount{ 0 };
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

      auto isLeaf() const -> bool { return m_TriCount > 0; }// empty BVH leaves do not exist
      static auto CalculateNodeCost(BVHNode &node) -> float;
    };


    class alignas(32) BVH : public IHittable<BVH> {

        struct BuildJob {
            uint32_t m_nodeidx;
            glm::vec3 m_centroidMin, m_centroidMax;
        };

    public:
        BVH() = default;

        explicit BVH(class IHittable<Mesh> *tri_mesh);

        void Build();

        auto Traversal(Ray &ray, float t_max) const -> bool;

        auto Hit(Ray &ray, float t_max) const -> bool;

        auto GetBounds(AABB &outbox) const -> AABB;


    private:
        void
        Subdivide(uint32_t nodeIdx, uint32_t depth, uint32_t &nodePtr, glm::vec3 &centroidMin, glm::vec3 &centroidMax);

        void UpdateNodeBounds(uint32_t nodeIdx, glm::vec3 &centroidMin, glm::vec3 &centroidMax) const;

        auto FindBestPlane(BVHNode &node, int &axis, int &splitPos, glm::vec3 &centroidMin,
                           glm::vec3 &centroidMax) const -> float;

    public:
        [[nodiscard]] auto DeepCopy() const -> std::shared_ptr<IHittable>;

        Mesh *m_mesh{};
        BVHNode *m_bvhNode{ nullptr };
        uint32_t *m_triIdx{ nullptr }, m_nodeCount{};
        std::array<BuildJob, 64> buildStack{};
        uint32_t buildStackPtr{};
    };


} // LumenRender

#endif //LUMEN_BVH_HPP