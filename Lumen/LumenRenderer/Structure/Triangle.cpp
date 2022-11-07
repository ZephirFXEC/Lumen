//
// Created by enzoc on 14/10/2022.
//

#include "Triangle.hpp"

namespace LumenRender {
    bool Triangle::Hit(Ray &ray, float t_max) const {
        const glm::vec3 edge1 = vertex1 - vertex0;
        const glm::vec3 edge2 = vertex2 - vertex0;
        const glm::vec3 pvec = glm::cross(ray.Direction, edge2);
        const float det = glm::dot(edge1, pvec);

        if (det < 0.0001f) return false;

        const float inv_det = 1.0f / det;
        glm::vec3 tvec = ray.Origin - vertex0;
        const float u = glm::dot(tvec, pvec) * inv_det;
        if (u < 0.0f || u > 1.0f) return false;

        glm::vec3 qvec = glm::cross(tvec, edge1);
        const float v = glm::dot(ray.Direction, qvec) * inv_det;
        if (v < 0.0f || u + v > 1.0f) return false;

        const float t = glm::dot(edge2, qvec) * inv_det;
        if (t < 0.0f || t > t_max) return false;

        ray.m_Record.m_T = t;
        ray.m_Record.m_Position = ray.At(t);
        ray.m_Record.m_Normal = glm::normalize(glm::cross(edge1, edge2));
        ray.m_Record.m_UVW = GetBarycentricCoordinates(ray.m_Record.m_Position);


        return true;
    }

    glm::vec3 Triangle::GetBarycentricCoordinates(const glm::vec3 &p) const {

        glm::vec3 v0 = vertex1 - vertex0, v1 = vertex2 - vertex0, v2 = p - vertex0;
        float den = v0.x * v1.y - v1.x * v0.y;
        float v = (v2.x * v1.y - v1.x * v2.y) / den;
        float w = (v0.x * v2.y - v2.x * v0.y) / den;
        float u = 1.0f - v - w;

        return {u, v, w};
    }


    bool Triangle::GetBounds(AABB &outbox) const {
        AABB one = AABB(vertex0, vertex1);
        AABB two = AABB(vertex1, vertex2);
        outbox = AABB::Union(one, two);
        return true;
    }


} // LumenRender