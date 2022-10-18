//
// Created by enzoc on 14/10/2022.
//

#ifndef LUMEN_TRIANGLE_HPP
#define LUMEN_TRIANGLE_HPP

#include "../Scene/Object.hpp"

namespace LumenRender {

    struct Vertex {
        glm::vec3 P;
        glm::vec3 N;
        glm::vec2 UV;
    };

    class Triangle : public Object {
    public:
        Triangle() = default;

        Triangle(const Vertex &v0, const Vertex &v1, const Vertex &v2) {
            _v0 = v0.P;
            _v1 = v1.P;
            _v2 = v2.P;

            _e1 = _v1 - _v0;
            _e2 = _v2 - _v0;

            _n = glm::normalize(glm::cross(_e1, _e2));

            UV[0] = v0.UV;
            UV[1] = v1.UV;
            UV[2] = v2.UV;

            N[0] = v0.N;
            N[1] = v1.N;
            N[2] = v2.N;

            _centroid = (_v0 + _v1 + _v2) / 3.0f;
        }

        bool Hit(const Ray &ray, float t_max, HitRecords &record) const override;
        bool Hit(const Ray &ray) const;

        bool GetBounds(AABB &outbox) const override;

        glm::vec3 GetBarycentricCoordinates(const glm::vec3 &p) const;

        glm::vec3 _centroid{};
    public:
        glm::vec3 _v0{}, _v1{}, _v2{};
        glm::vec3 _e1{}, _e2{};
        glm::vec3 _n{};
        glm::vec3 N[3]{};
        glm::vec2 UV[3]{};
    };

} // LumenRender

#endif //LUMEN_TRIANGLE_HPP