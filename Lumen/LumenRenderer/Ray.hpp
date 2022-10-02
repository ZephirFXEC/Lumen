//
// Created by enzoc on 01/10/2022.
//

#ifndef LUMEN_RAY_HPP
#define LUMEN_RAY_HPP

#include <glm/glm.hpp>

namespace LumenRender {

    struct RayDesc { // Ray descriptor organized like this for better memory alignment
        glm::vec3 Origin{};
        float Min = 0.0f;
        glm::vec3 Direction{};
        float Max = std::numeric_limits<float>::max();
    };

}
#endif //LUMEN_RAY_HPP