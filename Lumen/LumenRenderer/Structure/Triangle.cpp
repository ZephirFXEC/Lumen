//
// Created by enzoc on 14/10/2022.
//

#include "Triangle.hpp"

namespace LumenRender {
    bool Triangle::Hit(const Ray &ray, HitRecords &record) const {
        float u, v, temp;

        glm::vec3 pvec = glm::cross(ray.Direction, _e2);
        float det = glm::dot(_e1, pvec);

        if (det == 0.0f) return false;

        float inv_det = 1.0f / det;
        glm::vec3 tvec = ray.Origin - _v0;
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
                record.m_Normal = this->_n;

                glm::vec3 bary = GetBarycentricCoordinates(record.m_Position);
                record.m_Normal = glm::normalize((bary.x * N[0]) + (bary.y * N[1]) + bary.z * N[2]);

                return true;
            }
        }

        return false;
    }

    glm::vec3 Triangle::GetBarycentricCoordinates(const glm::vec3 &p) const {
        glm::vec3 v2_ = p - _v0;
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
        AABB one = AABB(_v0, _v1);
        AABB two = AABB(_v1, _v2);
        outbox = AABB::SurroundingBox(one, two);
        return true;
    }


} // LumenRender