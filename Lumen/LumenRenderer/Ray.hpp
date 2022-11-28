//
// Created by enzoc on 01/10/2022.
//

#ifndef LUMEN_RAY_HPP
#define LUMEN_RAY_HPP

#include <glm/glm.hpp>
#include <xmmintrin.h>

namespace LumenRender {

struct HitRecords
{
    glm::vec3 m_Normal;
    glm::vec2 m_UV;
    uint32_t m_PrimIndex;
    float m_T = -1.0F;
};

struct Ray
{// Ray descriptor organized like this for better memory alignment

    Ray() = default;
    Ray(const glm::vec3 &origin, const glm::vec3 &direction) : Origin(origin), Direction(direction) {}


    [[nodiscard]] auto At(float t) const -> glm::vec3 { return Origin + t * Direction; }

    glm::vec3 Origin{};
    glm::vec3 Direction{};

    HitRecords *m_Record{ nullptr };
};

}// namespace LumenRender
#endif// LUMEN_RAY_HPP