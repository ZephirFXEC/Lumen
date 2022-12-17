// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#include "Triangle.hpp"

#include <immintrin.h>

static auto _mm_cross_ps(__m128 a, __m128 n) -> __m128
{
    __m128 const a_yzx = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 const a_zxy = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 const n_yzx = _mm_shuffle_ps(n, n, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 const n_zxy = _mm_shuffle_ps(n, n, _MM_SHUFFLE(3, 1, 0, 2));
    return _mm_sub_ps(_mm_mul_ps(a_yzx, n_zxy), _mm_mul_ps(a_zxy, n_yzx));
}


namespace LumenRender {
auto Triangle::TriangleIntersect(const Ray &ray, const Triangle &tri, const uint32_t &primidx) -> bool
{
#ifndef SSE
    // Calculate pvec and det only once
    const glm::vec3 pvec = glm::cross(ray.Direction, tri._e2);
    const float det = glm::dot(tri._e1, pvec);

    if (det < 0.00001F) { return false; }// Avoid division by zero

    const float inv_det = 1.0F / det;
    glm::vec3 const tvec = ray.Origin - tri.vertex[0];
    const float u = glm::dot(tvec, pvec) * inv_det;

    if (u < 0.0F || u > 1.0F) { return false; }

    glm::vec3 const qvec = glm::cross(tvec, tri._e1);
    const float v = glm::dot(ray.Direction, qvec) * inv_det;
    if (v < 0.0F || u + v > 1.0F) { return false; }

    const float t = glm::dot(tri._e2, qvec) * inv_det;

    if (t < 0.0F || t > std::numeric_limits<float>::max()) { return false; }

    ray.m_Record->m_T = t;
    ray.m_Record->m_UV = glm::vec2(u, v);
    ray.m_Record->m_PrimIndex = primidx;

    return true;

#else
    __m128 const dir = _mm_set_ps(ray.Direction.x, ray.Direction.y, ray.Direction.z, 0.0F);
    __m128 const org = _mm_set_ps(ray.Origin.x, ray.Origin.y, ray.Origin.z, 0.0F);
    __m128 const e2 = _mm_set_ps(tri._e2.x, tri._e2.y, tri._e2.z, 0.0F);
    __m128 const e1 = _mm_set_ps(tri._e1.x, tri._e1.y, tri._e1.z, 0.0F);
    __m128 const v0 = _mm_set_ps(tri.vertex[0].x, tri.vertex[0].y, tri.vertex[0].z, 0.0F);

    __m128 const q = _mm_cross_ps(dir, e2);

    float a = _mm_dp_ps(e1, q, 0xFF).m128_f32[0];

    if (a > -0.00001 && a < 0.00001) return false;

    float const f = 1.0F / a;

    __m128 const s = _mm_sub_ps(org, v0);
    float u = f * _mm_dp_ps(s, q, 0xFF).m128_f32[0];

    if (u < 0.0f) return false;

    __m128 const r = _mm_cross_ps(s, e1);
    float v = f * _mm_dp_ps(dir, r, 0xFF).m128_f32[0];

    if (v < 0.0F || (u + v > 1.0F)) { return false; }

    float t = f * _mm_dp_ps(e2, r, 0xFF).m128_f32[0];
    if (t < 0.0F || t > std::numeric_limits<float>::max()) { return false; }

    ray.m_Record->m_T = t;
    ray.m_Record->m_UV = glm::vec2(u, v);
    ray.m_Record->m_PrimIndex = primidx;

    return true;

#endif
}

auto Triangle::GetBarycentricCoordinates(const glm::vec3 &p) const -> glm::vec3
{

    glm::vec3 const v2 = p - vertex.at(0);
    float const den = _e1.x * _e2.y - _e2.x * _e1.y;
    float const v = (v2.x * _e2.y - _e2.x * v2.y) / den;
    float const w = (_e1.x * v2.y - v2.x * _e1.y) / den;
    float const u = 1.0F - v - w;

    return { u, v, w };
}


auto Triangle::CalculateBounds(AABB &outbox) const -> AABB
{
    outbox = AABB(vertex.at(0), vertex.at(1), vertex.at(2));
    return outbox;
}
}// namespace LumenRender