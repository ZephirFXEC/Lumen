//
// Created by enzoc on 14/10/2022.
//

#ifndef LUMEN_TRIANGLE_HPP
#define LUMEN_TRIANGLE_HPP

#include "../Scene/Object.hpp"
#include <xmmintrin.h>

namespace LumenRender {


    __declspec(align(64)) struct TriData {
        glm::vec3 N[3];
        glm::vec3 UVW[3];
    };

    //TODO: Triangle isn't an object

    __declspec(align(64)) class Triangle : public Object {
    public:
        Triangle() = default;
        Triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2) :
                vertex0(v0), vertex1(v1), vertex2(v2), centroid((v0+v1+v2)/3.0f) {}


        bool Hit(Ray &ray, float t_max) const override;

        bool GetBounds(AABB &outbox) const override;

        [[nodiscard]] ObjectType GetType() const override { return ObjectType::TRIANGLE; }

        [[nodiscard]] glm::vec3 GetBarycentricCoordinates(const glm::vec3 &p) const;

    public:
        glm::vec3 vertex0{};
        glm::vec3 vertex1{};
        glm::vec3 vertex2{};
        glm::vec3 centroid{};
    };



} // LumenRender

#endif //LUMEN_TRIANGLE_HPP