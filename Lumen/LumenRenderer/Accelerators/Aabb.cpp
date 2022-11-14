//
// Created by enzoc on 15/10/2022.
//

#include "Aabb.hpp"

namespace LumenRender {
    auto AABB::IntersectAABB(const LumenRender::Ray &ray, const AABB &bounds) -> bool {
        const glm::vec3 t0 = {
                (bounds.pMin.x - ray.Origin.x) / ray.Direction.x,
                (bounds.pMin.y - ray.Origin.y) / ray.Direction.y,
                (bounds.pMin.z - ray.Origin.z) / ray.Direction.z,
        };
        const glm::vec3 t1 = {
                (bounds.pMax.x - ray.Origin.x) / ray.Direction.x,
                (bounds.pMax.y - ray.Origin.y) / ray.Direction.y,
                (bounds.pMax.z - ray.Origin.z) / ray.Direction.z,
        };

        const glm::vec3 t_smaller = {
                glm::min(t0.x, t1.x),
                glm::min(t0.y, t1.y),
                glm::min(t0.z, t1.z)
        };
        const glm::vec3 t_larger = {
                glm::max(t0.x, t1.x),
                glm::max(t0.y, t1.y),
                glm::max(t0.z, t1.z)
        };

        const float tmin = glm::max(t_smaller.x, glm::max(t_smaller.y, t_smaller.z));
        const float tmax = glm::min(t_larger.x, glm::min(t_larger.y, t_larger.z));

        return tmax > tmin;
    }

} // LumenRender