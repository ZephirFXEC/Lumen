// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#ifndef LUMEN_RAY_HPP
#define LUMEN_RAY_HPP

// #define GLM_FORCE_SSE2
// #define GLM_FORCE_ALIGNED

#include <glm/glm.hpp>

namespace LumenRender {

enum Type { HIT, MISS };

struct HitRecords
{
    Type mType = MISS;
    glm::vec3 mNormal = {};
    glm::vec2 mUV = {};
    uint32_t mPrimIndex = 0;
    float mT = -1.0F;
};

struct Ray
{
    glm::vec3 mDirection = {};
    glm::vec3 mOrigin = {};
    HitRecords *pRecord = nullptr;
};

}// namespace LumenRender
#endif// LUMEN_RAY_HPP