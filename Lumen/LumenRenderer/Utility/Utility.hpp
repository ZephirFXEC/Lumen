//
// Created by enzoc on 16/10/2022.
//

#ifndef LUMEN_UTILITY_HPP
#define LUMEN_UTILITY_HPP

#include <glm/glm.hpp>
#include <cmath>
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


#endif// LUMEN_UTILITY_HPP