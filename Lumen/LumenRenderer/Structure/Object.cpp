//
// Created by enzoc on 06/10/2022.
//

#include "Object.hpp"

#include <glm/gtx/intersect.hpp>

namespace LumenRender {
    bool Triangle::Intersect(const Ray &ray, HitRecords &hit) const {
        float t;
        glm::vec2 barycentric;
        if (glm::intersectRayTriangle(ray.Origin, ray.Direction, vert0, vert1, vert2, barycentric, t)) {
            hit.m_T = t;
            hit.m_Position = ray.At(t);
            hit.m_Normal = glm::normalize(glm::cross(vert1 - vert0, vert2 - vert0));

            return true;
        }
        return false;
    }
} // LumenRender