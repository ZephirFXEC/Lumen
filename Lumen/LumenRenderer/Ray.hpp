// s
//  Created by enzoc on 01/10/2022.
//

#ifndef LUMEN_RAY_HPP
#define LUMEN_RAY_HPP

#include <glm/glm.hpp>

namespace LumenRender {

struct alignas(32) HitRecords
{
    glm::vec3 m_Normal{};
    glm::vec2 m_UV{};
    uint32_t m_PrimIndex{};
    float m_T = -1.0F;
};

struct alignas(32) Ray
{// Ray descriptor organized like this for better memory alignment

    Ray() = default;
    Ray(const glm::vec3 &origin, const glm::vec3 &direction) : Direction(direction), Origin(origin) {}


    [[nodiscard]] auto At(float t) const -> glm::vec3 { return Origin + t * Direction; }

    glm::vec3 Direction{};
    glm::vec3 Origin{};
    HitRecords *m_Record{ nullptr };
};

}// namespace LumenRender
#endif// LUMEN_RAY_HPP