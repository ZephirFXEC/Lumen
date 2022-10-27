//
// Created by enzoc on 14/10/2022.
//

#ifndef LUMEN_TRIANGLE_HPP
#define LUMEN_TRIANGLE_HPP

#include "../Scene/Object.hpp"
#include <xmmintrin.h>
namespace LumenRender {

    __declspec(align(128)) struct Triangle : public Object {
        union { glm::vec3 vertex0{}; __m128 v0; };
        union { glm::vec3 vertex1{}; __m128 v1; };
        union { glm::vec3 vertex2{}; __m128 v2; };
        union { glm::vec3 centroid{}; __m128 centroid4; }; // total size: 64 bytes

        bool Hit(Ray &ray, float t_max) const override;
        bool GetBounds(AABB &outbox) const override;
        ObjectType GetType() const override { return ObjectType::TRIANGLE; }

        glm::vec3 GetBarycentricCoordinates(const glm::vec3 &p) const;
    };

    struct TriData {
        glm::vec3 N[3];
        glm::vec2 UV[3];
        char padding; // total size: 64 bytes
    };

} // LumenRender

#endif //LUMEN_TRIANGLE_HPP