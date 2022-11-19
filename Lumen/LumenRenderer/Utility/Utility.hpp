//
// Created by enzoc on 16/10/2022.
//

#ifndef LUMEN_UTILITY_HPP
#define LUMEN_UTILITY_HPP

#include <glm/glm.hpp>
#include <cmath>
#include <random>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <malloc.h>
#include <new>



template <typename T>
inline auto Random(T min, T max) -> T {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dis(min, max);
    return dis(gen);
}


#endif //LUMEN_UTILITY_HPP