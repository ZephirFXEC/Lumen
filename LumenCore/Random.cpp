//
// Created by enzoc on 30/09/2022.
//

#include "Random.hpp"

namespace Lumen {
    std::mt19937 Random::s_RandomEngine;
    std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;
}// namespace Lumen