//
// Created by enzoc on 01/10/2022.
//

#ifndef LUMEN_RAY_HPP
#define LUMEN_RAY_HPP

#include <glm/glm.hpp>

namespace LumenRender {

    struct HitRecords {
        glm::vec3 m_Position;
        uint32_t m_Index;
        glm::vec3 m_Normal;
        float m_T;
    };

    __declspec(align(64)) struct Ray { // Ray descriptor organized like this for better memory alignment

        Ray() = default;

        Ray(const glm::vec3 &origin, const glm::vec3 &direction, float tMin, float tMax)
                : Origin(origin), Direction(direction), Min(tMin), Max(tMax) {}

        [[nodiscard]] glm::vec3 At(float t) const { return Origin + t * Direction; }


        glm::vec3 Origin{};
        float Min = 0.0f;
        glm::vec3 Direction{};
        float Max = std::numeric_limits<float>::max();
        HitRecords m_Record{};
    }; // total size 64 bytes

}
#endif //LUMEN_RAY_HPP