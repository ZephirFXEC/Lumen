//
// Created by enzoc on 15/10/2022.
//

#include "Aabb.hpp"

namespace LumenRender {
    bool AABB::IntersectAABB(const LumenRender::Ray &ray, const AABB &bounds) {
        glm::vec3 invD = 1.0f / ray.Direction;

        float tx1 = (bounds.pMin.x - ray.Origin.x) * invD.x, tx2 = (bounds.pMax.x - ray.Origin.x) * invD.x;
        float tmin = std::min(tx1, tx2), tmax = std::max(tx1, tx2);

        float ty1 = (bounds.pMin.y - ray.Origin.y) * invD.y, ty2 = (bounds.pMax.y - ray.Origin.y) * invD.y;
        tmin = std::max(tmin, std::min(ty1, ty2)), tmax = std::min(tmax, std::max(ty1, ty2));

        float tz1 = (bounds.pMin.z - ray.Origin.z) * invD.z, tz2 = (bounds.pMax.z - ray.Origin.z) * invD.z;
        tmin = std::max(tmin, std::min(tz1, tz2)), tmax = std::min(tmax, std::max(tz1, tz2));
        return tmax >= tmin && tmin < ray.m_Record.m_T && tmax > 0;

    }

} // LumenRender