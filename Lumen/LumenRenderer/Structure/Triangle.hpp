//
// Created by enzoc on 14/10/2022.
//

#ifndef LUMEN_TRIANGLE_HPP
#define LUMEN_TRIANGLE_HPP

#include "../Scene/Object.hpp"
#include <array>
#include <xmmintrin.h>

namespace LumenRender {


struct alignas(32) TriData
{
  glm::vec3 N{};
  glm::vec2 UV{};
};


class alignas(64) Triangle
{
public:
  Triangle() = default;
  explicit Triangle(const std::array<glm::vec3, 3> &v)
    : vertex{ v }, Centroid((v[0] + v[1] + v[2]) / 3.F), _e1(v[1] - v[0]), _e2(v[2] - v[0])
  {}

  static auto TriangleIntersect(Ray &ray, const Triangle &tri, const uint32_t &primidx) -> bool;

  auto GetBounds(AABB &outbox) const -> AABB;

  [[nodiscard]] auto GetBarycentricCoordinates(const glm::vec3 &p) const -> glm::vec3;

  auto Transform(const glm::mat3 &transform) -> void;


  std::array<glm::vec3, 3> vertex{};
  glm::vec3 Centroid{};

private:
  glm::vec3 _e1{}, _e2{};
};



} // LumenRender

#endif //LUMEN_TRIANGLE_HPP