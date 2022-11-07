//
// Created by enzoc on 01/10/2022.
//

#ifndef LUMEN_RAY_HPP
#define LUMEN_RAY_HPP

#include <glm/glm.hpp>

namespace LumenRender {

    struct HitRecords {
        glm::vec3 m_Position;
        glm::vec3 m_Normal;
        glm::vec3 m_UVW;
        float m_T = 1e30;
    };

    __declspec(align(64)) struct Ray { // Ray descriptor organized like this for better memory alignment

        Ray() = default;

        Ray(const glm::vec3 &origin, const glm::vec3 &direction) :
                Origin(origin), Direction(direction) {}

        [[nodiscard]] glm::vec3 At(float t) const { return Origin + t * Direction; }


        glm::vec3 Origin{};
        glm::vec3 Direction{};
        HitRecords m_Record{};
    };

}
#endif //LUMEN_RAY_HPP