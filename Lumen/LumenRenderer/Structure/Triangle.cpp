//
// Created by enzoc on 14/10/2022.
//

#include "Triangle.hpp"

namespace LumenRender {
    bool Triangle::Hit(const Ray &ray, float t_max, HitRecords &record) const {
        float u, v, temp;
        glm::vec3 _e1 = vertex1 - vertex0;
        glm::vec3 _e2 = vertex2 - vertex0;

        glm::vec3 pvec = glm::cross(ray.Direction, _e2);
        float det = glm::dot(_e1, pvec);

        if (det == 0.0f) return false;

        float inv_det = 1.0f / det;
        glm::vec3 tvec = ray.Origin - vertex0;
        u = glm::dot(tvec, pvec) * inv_det;
        if (u < 0.0f || u > 1.0f) return false;

        glm::vec3 qvec = glm::cross(tvec, _e1);
        v = glm::dot(ray.Direction, qvec) * inv_det;
        if (v < 0.0f || u + v > 1.0f) return false;

        temp = glm::dot(_e2, qvec) * inv_det;
        if (temp < ray.Max) {
            if (temp > ray.Min) {
                record.m_T = temp;
                record.m_Position = ray.At(record.m_T);
                // TODO Proper Normals and UVs
                record.m_Normal = glm::normalize(glm::cross(_e1, _e2));

                return true;
            }
        }

        return false;
    }


    bool Triangle::GetBounds(AABB &outbox) const {
        AABB one = AABB(vertex0, vertex1);
        AABB two = AABB(vertex1, vertex2);
        outbox = AABB::Union(one, two);
        return true;
    }


} // LumenRender