//
// Created by enzoc on 01/10/2022.
//

#ifndef LUMEN_RAY_HPP
#define LUMEN_RAY_HPP

#include <glm/glm.hpp>

namespace LumenRender {

    const float MAX = std::numeric_limits<float>::max();

    struct HitRecords {
        glm::vec3 m_Position;
        glm::vec3 m_Normal;
        glm::vec2 m_UV;
        uint32_t m_PrimIndex;
        float m_T = MAX;
    };

    struct Ray { // Ray descriptor organized like this for better memory alignment

        Ray() = default;

        Ray(const glm::vec3 &origin, const glm::vec3 &direction) :
                Origin(origin), Direction(direction) {}

        [[nodiscard]] auto At(float t) const -> glm::vec3 { return Origin + t * Direction; }


        glm::vec3 Origin{};
        glm::vec3 Direction{};
        HitRecords m_Record{};
    };

}
#endif //LUMEN_RAY_HPP