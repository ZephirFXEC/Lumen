//
// Created by enzoc on 14/10/2022.
//

#include "Triangle.hpp"

namespace LumenRender {
    bool Triangle::Hit(Ray &ray, float t_max) const {
        glm::vec3 edge1 = vertex1 - vertex0;
        glm::vec3 edge2 = vertex2 - vertex0;
        glm::vec3 pvec = glm::cross(ray.Direction, edge2);
        float det = glm::dot(edge1, pvec);

        if (det < 0.0001f) return false;

        float inv_det = 1.0f / det;
        glm::vec3 tvec = ray.Origin - vertex0;
        float u = glm::dot(tvec, pvec) * inv_det;
        if (u < 0.0f || u > 1.0f) return false;

        glm::vec3 qvec = glm::cross(tvec, edge1);
        float v = glm::dot(ray.Direction, qvec) * inv_det;
        if (v < 0.0f || u + v > 1.0f) return false;

        float t = glm::dot(edge2, qvec) * inv_det;
        if (t < 0.0f || t > t_max) return false;

        ray.m_Record.m_T = t;
        ray.m_Record.m_Position = ray.At(t);
        ray.m_Record.m_Normal = glm::normalize(glm::cross(edge1, edge2));


        return true;
    }

    glm::vec3 Triangle::GetBarycentricCoordinates(const glm::vec3 &p) const {
        glm::vec3 v2_ = p - vertex0;
        glm::vec3 _e1 = vertex1 - vertex0;
        glm::vec3 _e2 = vertex2 - vertex0;
        float d00 = glm::dot(_e1, _e1);
        float d01 = glm::dot(_e1, _e2);
        float d11 = glm::dot(_e2, _e2);
        float d20 = glm::dot(v2_, _e1);
        float d21 = glm::dot(v2_, _e2);
        float d = d00 * d11 - d01 * d01;
        float v = (d11 * d20 - d01 * d21) / d;
        float w = (d00 * d21 - d01 * d20) / d;
        float u = 1 - v - w;
        return { u, v, w };
    }


    bool Triangle::GetBounds(AABB &outbox) const {
        AABB one = AABB(vertex0, vertex1);
        AABB two = AABB(vertex1, vertex2);
        outbox = AABB::Union(one, two);
        return true;
    }


} // LumenRender