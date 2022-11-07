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

        const glm::vec3 &operator[](int i) const;

        glm::vec3 &operator[](int i);

        bool operator==(const AABB &b) const {
            return b.pMin == pMin && b.pMax == pMax;
        }

        bool operator!=(const AABB &b) const {
            return b.pMin != pMin || b.pMax != pMax;
        }


        static AABB Union(const AABB &a, const AABB &b) {
            AABB ret;
            ret.pMin = { glm::min(a.pMin.x, b.pMin.x), glm::min(a.pMin.y, b.pMin.y), glm::min(a.pMin.z, b.pMin.z) };
            ret.pMax = { glm::max(a.pMax.x, b.pMax.x), glm::max(a.pMax.y, b.pMax.y), glm::max(a.pMax.z, b.pMax.z) };
            return ret;
        }

        static AABB Union(const AABB &a, const glm::vec3 &p) {
            AABB ret;
            ret.pMin = { glm::min(a.pMin.x, p.x), glm::min(a.pMin.y, p.y), glm::min(a.pMin.z, p.z) };
            ret.pMax = { glm::max(a.pMax.x, p.x), glm::max(a.pMax.y, p.y), glm::max(a.pMax.z, p.z) };
            return ret;
        }

         static AABB Union(const glm::vec3 &p, const glm::vec3 &p2) {
            AABB ret;
            ret.pMin = { glm::min(p.x, p2.x), glm::min(p.y, p2.y), glm::min(p.z, p2.z) };
            ret.pMax = { glm::max(p.x, p2.x), glm::max(p.y, p2.y), glm::max(p.z, p2.z) };
            return ret;
         }


        [[nodiscard]] glm::vec3 Diagonal() const {
            return pMax - pMin;
        }

        [[nodiscard]] float SurfaceArea() const {
            glm::vec3 d = pMax - pMin;
            return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
        }


        [[nodiscard]] static bool IntersectAABB(const LumenRender::Ray &ray, const AABB& bounds);



        glm::vec3 pMin{};
        glm::vec3 pMax{};

    };

} // LumenRender

#endif //LUMEN_AABB_HPP