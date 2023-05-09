// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#ifndef LUMEN_TRIANGLE_HPP
#define LUMEN_TRIANGLE_HPP


#include "../Accelerators/Aabb.hpp"

#include <array>
#include <optional>


namespace LumenRender {


struct alignas(32) TriData
{
    glm::vec3 N = {};
    glm::vec2 UV = {};
    glm::vec3 Centroid = {};
};


class Triangle
{
  public:
    Triangle() = default;

    explicit Triangle(const glm::vec3 *v) : mVertex{ v[0], v[1], v[2] }, _e1(v[1] - v[0]), _e2(v[2] - v[0]) {}

    static auto TriangleIntersect(const Ray &ray, const Triangle &tri, const uint32_t &primidx) -> bool;

    auto CalculateBounds(AABB &outbox) const -> AABB;

    [[nodiscard]] auto GetBarycentricCoordinates(const glm::vec3 &p) const -> glm::vec3;


    glm::vec3 mVertex[3] = {};
    TriData *pData = nullptr;

  private:
    glm::vec3 _e1 = {};
    glm::vec3 _e2 = {};
};


}// namespace LumenRender

#endif// LUMEN_TRIANGLE_HPP