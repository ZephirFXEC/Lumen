//
// Created by enzoc on 15/10/2022.
//

#ifndef LUMEN_AABB_HPP
#define LUMEN_AABB_HPP

#include <glm/glm.hpp>
#include "../Ray.hpp"

namespace LumenRender {

    class AABB {
    public:
        AABB() {
            float minNum = std::numeric_limits<float>::lowest();
            float maxNum = std::numeric_limits<float>::max();
            pMin = {maxNum, maxNum, maxNum};
            pMax = {minNum, minNum, minNum};
        }

        explicit AABB(const glm::vec3 &p) : pMin(p), pMax(p) {}

        AABB(const glm::vec3 &p1, const glm::vec3 &p2) {
            pMin = {std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)};
            pMax = {std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z)};
        }

        // Constructor for Triangle AABBs
        AABB(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
            pMin = {std::min(p1.x, std::min(p2.x, p3.x)), std::min(p1.y, std::min(p2.y, p3.y)), std::min(p1.z, std::min(p2.z, p3.z))};
            pMax = {std::max(p1.x, std::max(p2.x, p3.x)), std::max(p1.y, std::max(p2.y, p3.y)), std::max(p1.z, std::max(p2.z, p3.z))};
        }


        auto operator[](int i) const -> const glm::vec3 &;

        auto operator[](int i) -> glm::vec3 &;

        auto operator==(const AABB &b) const -> bool {
            return b.pMin == pMin && b.pMax == pMax;
        }

        auto operator!=(const AABB &b) const -> bool {
            return b.pMin != pMin || b.pMax != pMax;
        }


        static auto Union(const AABB &a, const AABB &b) -> AABB {
            AABB ret;
            ret.pMin = { std::min(a.pMin.x, b.pMin.x), std::min(a.pMin.y, b.pMin.y), std::min(a.pMin.z, b.pMin.z) };
            ret.pMax = { std::max(a.pMax.x, b.pMax.x), std::max(a.pMax.y, b.pMax.y), std::max(a.pMax.z, b.pMax.z) };
            return ret;
        }

        static auto Union(const AABB &a, const glm::vec3 &p) -> AABB {
            AABB ret;
            ret.pMin = { std::min(a.pMin.x, p.x), std::min(a.pMin.y, p.y), std::min(a.pMin.z, p.z) };
            ret.pMax = { std::max(a.pMax.x, p.x), std::max(a.pMax.y, p.y), std::max(a.pMax.z, p.z) };
            return ret;
        }

         static auto Union(const glm::vec3 &p, const glm::vec3 &p2) -> AABB {
            AABB ret;
            ret.pMin = { std::min(p.x, p2.x), std::min(p.y, p2.y), std::min(p.z, p2.z) };
            ret.pMax = { std::max(p.x, p2.x), std::max(p.y, p2.y), std::max(p.z, p2.z) };
            return ret;
         }


        [[nodiscard]] auto Diagonal() const -> glm::vec3 {
            return pMax - pMin;
        }

        [[nodiscard]] auto SurfaceArea() const -> float {
            glm::vec3 d = pMax - pMin;
            return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
        }


        [[nodiscard]] static auto IntersectAABB(const LumenRender::Ray &ray, const AABB& bounds) -> bool;



        glm::vec3 pMin{};
        glm::vec3 pMax{};
    };

} // LumenRender

#endif //LUMEN_AABB_HPP