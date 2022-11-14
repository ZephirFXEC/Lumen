//
// Created by enzoc on 14/10/2022.
//

#ifndef LUMEN_TRIANGLE_HPP
#define LUMEN_TRIANGLE_HPP

#include "../Scene/Object.hpp"
#include <array>
#include <xmmintrin.h>

namespace LumenRender {


    struct TriData {
        glm::vec3 N{};
        glm::vec2 UV{};
        glm::vec3 Centroid{};
    };


    class Triangle {
    public:
        Triangle() = default;
        Triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2) :
                vertex0(v0), vertex1(v1), vertex2(v2), _e1(v1 - v0), _e2(v2 - v0) {}


        static auto TriangleIntersect(Ray &ray, Triangle* tri, const uint32_t& primidx) -> bool;

        auto GetBounds(AABB &outbox) const -> bool;

        [[nodiscard]] auto GetBarycentricCoordinates(const glm::vec3 &p) const -> glm::vec3;


        glm::vec3 vertex0{};
        glm::vec3 vertex1{};
        glm::vec3 vertex2{};

    private:
        glm::vec3 _e1{}, _e2{};
    };



} // LumenRender

#endif //LUMEN_TRIANGLE_HPP