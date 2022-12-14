//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_RANDOM_HPP
#define LUMEN_RANDOM_HPP

#include <glm/glm.hpp>
#include <random>

namespace Lumen {

class Random
{
public:
  [[maybe_unused]] static void Init() { s_RandomEngine.seed(std::random_device()()); }

  [[maybe_unused]] static auto UInt() -> uint32_t { return s_Distribution(s_RandomEngine); }

  [[maybe_unused]] static auto UInt(uint32_t min, uint32_t max) -> uint32_t
  {
    return min + (s_Distribution(s_RandomEngine) % (max - min + 1));
  }

  static auto Float() -> float
  {
    return static_cast<float>(s_Distribution(s_RandomEngine))
           / static_cast<float>(std::numeric_limits<uint32_t>::max());
  }

  [[maybe_unused]] static auto Vec3() -> glm::vec3 { return { Float(), Float(), Float() }; }

  static auto Vec3(float min, float max) -> glm::vec3
  {
    return { Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min };
  }

  [[maybe_unused]] static auto InUnitSphere() -> glm::vec3 { return glm::normalize(Vec3(-1.0F, 1.0F)); }

private:
  static  std::mt19937 s_RandomEngine;
  static  std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};

}// namespace Lumen

#endif// LUMEN_RANDOM_HPP