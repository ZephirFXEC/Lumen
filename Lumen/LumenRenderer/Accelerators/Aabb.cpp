//
// Created by enzoc on 15/10/2022.
//

#include "Aabb.hpp"
#include "../Utility/Utility.hpp"
#include <array>

namespace LumenRender {
auto AABB::IntersectAABB(const LumenRender::Ray &ray, const glm::vec3 &min, const glm::vec3 max) -> float
{

    glm::vec3 const invdir = 1.0F / ray.Direction;

    glm::vec3 const t0s = (min - ray.Origin) * invdir;
    glm::vec3 const t1s = (max - ray.Origin) * invdir;

    glm::vec3 tsmaller = glm::min(t0s, t1s);
    glm::vec3 tbigger = glm::max(t0s, t1s);

    float const tmin = std::max(tsmaller[0], std::max(tsmaller[1], tsmaller[2]));
    float const tmax = std::min(tbigger[0], std::min(tbigger[1], tbigger[2]));

    return tmin < tmax ? tmin : 1e30F;
}

auto AABB::IntersectAABB_SSE(const Ray &ray, const __m128 &min4, const __m128 &max4) -> float
{

    static __m128 const mask4 = _mm_cmpeq_ps(_mm_setzero_ps(), _mm_set_ps(1, 0, 0, 0));
    __m128 const t1 = _mm_mul_ps(_mm_sub_ps(_mm_and_ps(min4, mask4), ray.O4), ray.inv_D4);
    __m128 const t2 = _mm_mul_ps(_mm_sub_ps(_mm_and_ps(max4, mask4), ray.O4), ray.inv_D4);
    __m128 const vmax4 = _mm_max_ps(t1, t2);
    __m128 const vmin4 = _mm_min_ps(t1, t2);
    float tmax = std::min(vmax4.m128_f32[0], std::min(vmax4.m128_f32[1], vmax4.m128_f32[2]));
    float tmin = std::max(vmin4.m128_f32[0], std::max(vmin4.m128_f32[1], vmin4.m128_f32[2]));
    return (tmax >= tmin) ? tmin : 1e30F;
}

}// namespace LumenRender