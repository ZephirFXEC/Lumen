//
// Created by enzoc on 01/10/2022.
//

#ifndef LUMEN_RAY_HPP
#define LUMEN_RAY_HPP

#include <glm/glm.hpp>
#include <xmmintrin.h>

namespace LumenRender {

const float MAX = std::numeric_limits<float>::max();

struct HitRecords
{
    glm::vec3 m_Position;
    glm::vec3 m_Normal;
    glm::vec2 m_UV;
    uint32_t m_PrimIndex;
    float m_T = -1.0F;
};

struct Ray
{// Ray descriptor organized like this for better memory alignment

    Ray() { O4 = D4 = inv_D4 = _mm_set1_ps(1); }

    Ray(const glm::vec3 &origin, const glm::vec3 &direction) : Origin(origin), Direction(direction)
    {
        O4 = _mm_set_ps(0, origin.z, origin.y, origin.x);
        D4 = _mm_set_ps(0, direction.z, direction.y, direction.x);
        inv_D4 = _mm_set_ps(0, inv_Direction.z, inv_Direction.y, inv_Direction.x);
    }


    [[nodiscard]] auto At(float t) const -> glm::vec3 { return Origin + t * Direction; }

    union {
        struct
        {
            glm::vec3 Origin;
            float _dummy1;
        };
        __m128 O4;
    };
    union {
        struct
        {
            glm::vec3 Direction;
            float _dummy2;
        };
        __m128 D4;
    };
    union {
        struct
        {
            glm::vec3 inv_Direction;
            float _dummy3;
        };
        __m128 inv_D4;
    };

    HitRecords m_Record{};
};

}// namespace LumenRender
#endif// LUMEN_RAY_HPP