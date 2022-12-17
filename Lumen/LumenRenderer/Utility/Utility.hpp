// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#ifndef LUMEN_UTILITY_HPP
#define LUMEN_UTILITY_HPP

#include <glm/glm.hpp>
#include <random>


template<typename T> inline auto Random(T min, T max) -> T
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dis(min, max);
    return dis(gen);
}

template<typename T> static auto MinComponent(const T &v) -> float { return std::min(v[0], std::min(v[1], v[2])); }

template<typename T> static auto MaxComponent(const T &v) -> float { return std::max(v[0], std::max(v[1], v[2])); }

template<typename T> static auto DominantAxis(const T &v) -> uint8_t
{
    float x = fabs(v.x), y = fabs(v.y), z = fabs(v.z);
    float m = glm::max(glm::max(x, y), z);
    return m == x ? 0 : (m == y ? 1 : 2);
}

#endif// LUMEN_UTILITY_HPP