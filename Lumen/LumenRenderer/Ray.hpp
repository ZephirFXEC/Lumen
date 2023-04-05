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
        float m_T;
    };

    struct Ray {

        Ray() = default;

        Ray(const glm::vec3 &origin, const glm::vec3 &direction, float tMin, float tMax)
                : Origin(origin), Min(tMin), Direction(direction), Max(tMax) {}


        glm::vec3 Origin{};
        float Min{0.0f};
        glm::vec3 Direction{};
        float Max{std::numeric_limits<float>::max()};
    };

}
#endif //LUMEN_RAY_HPP