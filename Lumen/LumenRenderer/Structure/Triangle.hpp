//
// Created by enzoc on 14/10/2022.
//

#ifndef LUMEN_TRIANGLE_HPP
#define LUMEN_TRIANGLE_HPP

#include "../Scene/Object.hpp"
#include <xmmintrin.h>

namespace LumenRender {

    struct Triangle : public Object {

        glm::vec3 vertex0{};
        glm::vec3 vertex1{};
        glm::vec3 vertex2{};
        glm::vec3 centroid{}; // total size: 64 bytes

        bool Hit(Ray &ray, float t_max) const override;

        bool GetBounds(AABB &outbox) const override;

        [[nodiscard]] ObjectType GetType() const override { return ObjectType::TRIANGLE; }

        [[nodiscard]] glm::vec3 GetBarycentricCoordinates(const glm::vec3 &p) const;
    };

    __declspec(align(64)) struct TriData {
        glm::vec3 N[3];
        glm::vec2 UV[3];
    };

} // LumenRender

#endif //LUMEN_TRIANGLE_HPP