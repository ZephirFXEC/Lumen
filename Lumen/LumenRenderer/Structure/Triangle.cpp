//
// Created by enzoc on 14/10/2022.
//

#include "Triangle.hpp"

namespace LumenRender {
    auto Triangle::TriangleIntersect(Ray &ray, const Triangle& tri, const uint32_t &primidx) -> bool {

        const glm::vec3 pvec = glm::cross(ray.Direction, tri._e2);
        const float det = glm::dot(tri._e1, pvec);

        if (det < 0.00001F) {
            return false;
        }

        const float inv_det = 1.0F / det;
        glm::vec3 const tvec = ray.Origin - tri.vertex[0];
        const float u = glm::dot(tvec, pvec) * inv_det;
        if (u < 0.0F || u > 1.0F) {
            return false;
        }

        glm::vec3 const qvec = glm::cross(tvec, tri._e1);
        const float v = glm::dot(ray.Direction, qvec) * inv_det;
        if (v < 0.0F || u + v > 1.0F) {
            return false;
        }

        const float t = glm::dot(tri._e2, qvec) * inv_det;
        if (t < 0.0F || t > std::numeric_limits<float>::max()) {
            return false;
        }

        ray.m_Record.m_T = t;
        ray.m_Record.m_Position = ray.At(t);
        ray.m_Record.m_UV = glm::vec2(u, v);
        ray.m_Record.m_PrimIndex = primidx;

        return true;
    }

    auto Triangle::GetBarycentricCoordinates(const glm::vec3 &p) const -> glm::vec3 {

        glm::vec3 const v2 = p - vertex[0];
        float const den = _e1.x * _e2.y - _e2.x * _e1.y;
        float const v = (v2.x * _e2.y - _e2.x * v2.y) / den;
        float const w = (_e1.x * v2.y - v2.x * _e1.y) / den;
        float const u = 1.0F - v - w;

        return { u, v, w };
    }


    auto Triangle::GetBounds(AABB &outbox) const -> AABB {
        outbox = AABB(vertex[0], vertex[1], vertex[2]);
        return outbox;
    }


} // LumenRender