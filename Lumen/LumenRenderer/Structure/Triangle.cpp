// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#include "Triangle.hpp"

#include <immintrin.h>

namespace LumenRender {
auto Triangle::TriangleIntersect(const Ray &ray, const Triangle &tri, const uint32_t &primidx) -> bool
{
    // Calculate pvec and det only once
    const glm::vec3 pvec = glm::cross(ray.mDirection, tri._e2);
    const float det = glm::dot(tri._e1, pvec);

    if (det < 0.00001F) { return false; }// Avoid division by zero

    const float inv_det = 1.0F / det;
    const glm::vec3 tvec = ray.mOrigin - tri.mVertex[0];
    const float u = glm::dot(tvec, pvec) * inv_det;

    if (u < 0.0F || u > 1.0F) { return false; }

    const glm::vec3 qvec = glm::cross(tvec, tri._e1);
    const float v = glm::dot(ray.mDirection, qvec) * inv_det;
    if (v < 0.0F || u + v > 1.0F) { return false; }

    const float t = glm::dot(tri._e2, qvec) * inv_det;

    if (t < 0.0F || t > INFINITY) { return false; }

    ray.pRecord->mT = t;
    ray.pRecord->mUV = glm::vec2(u, v);
    ray.pRecord->mPrimIndex = primidx;

    return true;
}

auto Triangle::GetBarycentricCoordinates(const glm::vec3 &p) const -> glm::vec3
{

    glm::vec3 const v2 = p - mVertex[0];
    float const den = _e1.x * _e2.y - _e2.x * _e1.y;
    float const v = (v2.x * _e2.y - _e2.x * v2.y) / den;
    float const w = (_e1.x * v2.y - v2.x * _e1.y) / den;
    float const u = 1.0F - v - w;

    return { u, v, w };
}


auto Triangle::CalculateBounds(AABB &outbox) const -> AABB
{
    outbox = AABB(mVertex[0], mVertex[1], mVertex[2]);
    return outbox;
}
}// namespace LumenRender