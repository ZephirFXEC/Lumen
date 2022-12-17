// Copyright (c) 2022.
// Enzo Crema
// All rights reserved

#ifndef LUMEN_AABB_HPP
#define LUMEN_AABB_HPP

#include "../Ray.hpp"

constexpr float INF = 1e30F;

namespace LumenRender {

class AABB
{
  public:
    constexpr AABB() : pMin(glm::vec3(0.0F)), pMax(glm::vec3(0.0F)) {}
    explicit constexpr AABB(const glm::vec3 &p) : pMin(p), pMax(p) {}


    constexpr AABB(const glm::vec3 &p1, const glm::vec3 &p2)
      : pMin({ std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z) }),
        pMax({ std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z) })
    {}


    // Constructor for Triangle AABBs
    constexpr AABB(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
      : pMin({ std::min(p1.x, std::min(p2.x, p3.x)),
        std::min(p1.y, std::min(p2.y, p3.y)),
        std::min(p1.z, std::min(p2.z, p3.z)) }),
        pMax({ std::max(p1.x, std::max(p2.x, p3.x)),
          std::max(p1.y, std::max(p2.y, p3.y)),
          std::max(p1.z, std::max(p2.z, p3.z)) })
    {}

    constexpr auto operator[](int i) const -> const glm::vec3 & { return (i == 0) ? pMin : pMax; }

    constexpr auto operator[](int i) -> glm::vec3 & { return (i == 0) ? pMin : pMax; }

    constexpr auto operator==(const AABB &b) const -> bool { return b.pMin == pMin && b.pMax == pMax; }

    constexpr auto operator!=(const AABB &b) const -> bool { return b.pMin != pMin || b.pMax != pMax; }


    constexpr static auto Union(const AABB &a, const AABB &b) -> AABB
    {
        return { { std::min(a.pMin.x, b.pMin.x), std::min(a.pMin.y, b.pMin.y), std::min(a.pMin.z, b.pMin.z) },
            { std::max(a.pMax.x, b.pMax.x), std::max(a.pMax.y, b.pMax.y), std::max(a.pMax.z, b.pMax.z) } };
    }

    constexpr static auto Union(const AABB &a, const glm::vec3 &p) -> AABB
    {
        return { { std::min(a.pMin.x, p.x), std::min(a.pMin.y, p.y), std::min(a.pMin.z, p.z) },
            { std::max(a.pMax.x, p.x), std::max(a.pMax.y, p.y), std::max(a.pMax.z, p.z) } };
    }

    constexpr static auto Union(const glm::vec3 &p, const glm::vec3 &p2) -> AABB
    {
        return { { std::min(p.x, p2.x), std::min(p.y, p2.y), std::min(p.z, p2.z) },
            { std::max(p.x, p2.x), std::max(p.y, p2.y), std::max(p.z, p2.z) } };
    }

    [[nodiscard]] constexpr auto Union(const glm::vec3 &p) const -> AABB
    {
        return { { std::min(pMin.x, p.x), std::min(pMin.y, p.y), std::min(pMin.z, p.z) },
            { std::max(pMax.x, p.x), std::max(pMax.y, p.y), std::max(pMax.z, p.z) } };
    }


    [[nodiscard]] constexpr auto Diagonal() const -> glm::vec3 { return pMax - pMin; }

    [[nodiscard]] constexpr auto SurfaceArea() const -> float
    {
        glm::vec3 const d = pMax - pMin;
        return d.x * d.y + d.x * d.z + d.y * d.z;
    }


    [[nodiscard]] static auto
      IntersectAABB(const LumenRender::Ray &ray, const glm::vec3 &invdir, const glm::vec3 &min, const glm::vec3 &max)
        -> float;


    glm::vec3 pMin{};
    glm::vec3 pMax{};
};

}// namespace LumenRender

#endif// LUMEN_AABB_HPP