//
// Created by enzoc on 14/10/2022.
//

#include "Triangle.hpp"

namespace LumenRender {
    bool Triangle::TriangleIntersect(Ray &ray, Triangle* tri, const uint32_t& primidx) {
        const glm::vec3 pvec = glm::cross(ray.Direction, tri->_e2);
        const float det = glm::dot(tri->_e1, pvec);

        if (det < 0.0001f) return false;

        const float inv_det = 1.0f / det;
        glm::vec3 tvec = ray.Origin - tri->vertex0;
        const float u = glm::dot(tvec, pvec) * inv_det;
        if (u < 0.0f || u > 1.0f) return false;

        glm::vec3 qvec = glm::cross(tvec, tri->_e1);
        const float v = glm::dot(ray.Direction, qvec) * inv_det;
        if (v < 0.0f || u + v > 1.0f) return false;

        const float t = glm::dot(tri->_e2, qvec) * inv_det;
        if (t < 0.0f || t > std::numeric_limits<float>::max()) return false;

        ray.m_Record.m_T = t;
        ray.m_Record.m_Position = ray.At(t);
        ray.m_Record.m_UV = glm::vec2(u, v);
        ray.m_Record.m_PrimIndex = primidx;

        return true;
    }

    glm::vec3 Triangle::GetBarycentricCoordinates(const glm::vec3 &p) const {

        glm::vec3 v2 = p - vertex0;
        float den = _e1.x * _e2.y - _e2.x * _e1.y;
        float v = (v2.x * _e2.y - _e2.x * v2.y) / den;
        float w = (_e1.x * v2.y - v2.x * _e1.y) / den;
        float u = 1.0f - v - w;

        return {u, v, w};
    }


    bool Triangle::GetBounds(AABB &outbox) const {
        outbox = AABB(vertex0, vertex1, vertex2);
        return true;
    }


} // LumenRender