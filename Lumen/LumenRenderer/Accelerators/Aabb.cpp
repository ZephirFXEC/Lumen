//
// Created by enzoc on 15/10/2022.
//

#include "Aabb.hpp"

namespace LumenRender {
    auto AABB::IntersectAABB(const LumenRender::Ray &ray, const glm::vec3& min, const glm::vec3 max) -> float {

        glm::vec3 const inv_Direction = 1.0F / ray.Direction;

        float const tx1 = (min.x - ray.Origin.x) * inv_Direction.x;
        float const tx2 = (max.x - ray.Origin.x) * inv_Direction.x;

        float tmin = std::min( tx1, tx2 );
        float tmax = std::max( tx1, tx2 );

        float const ty1 = (min.y - ray.Origin.y) * inv_Direction.y;
        float const ty2 = (max.y - ray.Origin.y) * inv_Direction.y;
        tmin = std::max( tmin, std::min( ty1, ty2 ) );
        tmax = std::min( tmax, std::max( ty1, ty2 ) );

        float const tz1 = (min.z - ray.Origin.z) * inv_Direction.z;
        float const tz2 = (max.z - ray.Origin.z) * inv_Direction.z;
        tmin = std::max( tmin, std::min( tz1, tz2 ) );
        tmax = std::min( tmax, std::max( tz1, tz2 ) );

        return ( tmax >= tmin ) ? tmin : 1e30F;
    }

    auto AABB::IntersectAABB_SEE(const Ray &ray, const __m128 &min4, const __m128 &max4) -> float {

        glm::vec3 const inv_Direction = 1.0F / ray.Direction;
        __m128 const inv_Direction4 = _mm_set_ps(inv_Direction.z, inv_Direction.y, inv_Direction.x, 0.0F);

        static __m128 const mask4 = _mm_cmpeq_ps( _mm_setzero_ps(), _mm_set_ps( 1, 0, 0, 0 ) );
        __m128 const t1 = _mm_mul_ps( _mm_sub_ps( _mm_and_ps( min4, mask4 ), ray.O4 ), inv_Direction4 );
        __m128 const t2 = _mm_mul_ps( _mm_sub_ps( _mm_and_ps( max4, mask4 ), ray.O4 ), inv_Direction4 );
        __m128 const vmax4 = _mm_max_ps( t1, t2 );
        __m128 const vmin4 = _mm_min_ps( t1, t2 );
        float tmax = std::min( vmax4.m128_f32[0], std::min( vmax4.m128_f32[1], vmax4.m128_f32[2] ) );
        float tmin = std::max( vmin4.m128_f32[0], std::max( vmin4.m128_f32[1], vmin4.m128_f32[2] ) );
        return ( tmax >= tmin ) ? tmin : 1e30F;
    }

} // LumenRender