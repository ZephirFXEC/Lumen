//
// Created by enzoc on 15/10/2022.
//

#include "Aabb.hpp"

namespace LumenRender {
    auto AABB::IntersectAABB(const LumenRender::Ray &ray, const AABB &bounds) -> float {
        auto t_min = 0.0F;
        auto t_max = std::numeric_limits<float>::max();

        for (int a = 0; a < 3; a++) {
            const auto invD = 1.0F / ray.Direction[a];
            auto t0 = (bounds.pMin[a] - ray.Origin[a]) * invD;
            auto t1 = (bounds.pMax[a] - ray.Origin[a]) * invD;
            if (invD < 0.0F) {
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min) {
                return 1e30F;
            }
        }
        return t_min;
    }

} // LumenRender