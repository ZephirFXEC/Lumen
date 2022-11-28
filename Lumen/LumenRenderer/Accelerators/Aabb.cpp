//
// Created by enzoc on 15/10/2022.
//

#include "Aabb.hpp"
#include <array>

namespace LumenRender {
auto AABB::IntersectAABB(const LumenRender::Ray &ray, const glm::vec3 &min, const glm::vec3 &max) -> float
{

    glm::vec3 const invdir = 1.0F / ray.Direction;

    float tmin = 0;
    float tmax = INF;

    for (int j = 0; j < 3; ++j) {
        float const t1 = (min[j] - ray.Origin[j]) * invdir[j];
        float const t2 = (max[j] - ray.Origin[j]) * invdir[j];

        tmin = std::min(std::max(t1, tmin), std::max(t2, tmin));
        tmax = std::max(std::min(t1, tmax), std::min(t2, tmax));
    }

    return tmin < tmax ? tmin : 1e30F;
}

}// namespace LumenRender