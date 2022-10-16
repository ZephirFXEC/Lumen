//
// Created by enzoc on 15/10/2022.
//

#include "Aabb.hpp"

namespace LumenRender {
    bool AABB::Hit(const LumenRender::Ray &ray, float& t) const {
        for (int a = 0; a < 3; a++) {
            float invD = 1.0f / ray.Direction[a];
            float t0 = (pMin[a] - ray.Origin[a]) * invD;
            float t1 = (pMax[a] - ray.Origin[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t = t0 > t ? t0 : t;
            if (t > t1)
                return false;
        }
        return true;
    }

} // LumenRender