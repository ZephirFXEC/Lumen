//
// Created by enzoc on 14/10/2022.
//

#include "Triangle.hpp"
#include <smmintrin.h>
#include <xmmintrin.h>


namespace LumenRender {
auto Triangle::TriangleIntersect(Ray &ray, const Triangle &tri, const uint32_t &primidx) -> bool
{
#if 1
    const glm::vec3 pvec = glm::cross(ray.Direction, tri._e2);
    const float det = glm::dot(tri._e1, pvec);

    if (det < 0.00001F) { return false; }

    const float inv_det = 1.0F / det;
    glm::vec3 const tvec = ray.Origin - tri.vertex.at(0);
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
    // Pre-compute some values that will be used in the intersection calculation
    const glm::vec3 pvec = glm::cross(ray.Direction, tri._e2);
    const glm::vec3 tvec = ray.Origin - tri.vertex.at(0);

    // Use SIMD instructions to calculate the dot products of e1 and pvec, and tvec and e1
    const glm::vec3 dot_e1_pvec = _mm_dp_ps(tri._e1, pvec, 0x71);
    const glm::vec3 dot_tvec_e1 = _mm_dp_ps(tvec, tri._e1, 0x71);

    // Extract the first element of the dot product vectors and use it to calculate the determinant
    const float det = dot_e1_pvec[0];

    // If the determinant is less than a small threshold, return false
    if (det < 0.00001F) { return false; }

    // Calculate the inverse determinant
    const float inv_det = 1.0F / det;

    // Use SIMD instructions to calculate the dot product of tvec and pvec
    const glm::vec3 dot_tvec_pvec = _mm_dp_ps(tvec, pvec, 0x71);

    // Extract the first element of the dot product vector and use it to calculate the u coordinate
    const float u = dot_tvec_pvec[0] * inv_det;
    if (u < 0.0F || u > 1.0F) { return false; }

    // Calculate the cross product of tvec and e1
    glm::vec3 const qvec = glm::cross(tvec, tri._e1);

    // Use SIMD instructions to calculate the dot product of ray.Direction and qvec
    const glm::vec3 dot_dir_qvec = _mm_dp_ps(ray.Direction, qvec, 0x71);

    // Extract the first element of the dot product vector and use it to calculate the v coordinate
    const float v = dot_dir_qvec[0] * inv_det;
    if (v < 0.0F || u + v > 1.0F) { return false; }

    // Use SIMD instructions to calculate the dot product of e2 and qvec
    const glm::vec3 dot_e2_qvec = _mm_dp_ps(tri._e2, qvec, 0x71);

    // Extract the first element of the dot product vector and use it to calculate the t coordinate
    const float t = dot_e2_qvec[0] * inv_det;
    if (t < 0.0F || t > std::numeric_limits<float>::max()) { return false; }

    // Update the ray record with the intersection information
    ray.m_Record->m_T = t;
    ray.m_Record->m_UV = glm::vec2(u, v);
    ray.m_Record->m
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

auto Triangle::Transform(const glm::mat3 &transform) -> void
{
    vertex.at(0) = transform * vertex.at(0);
    vertex.at(1) = transform * vertex.at(1);
    vertex.at(2) = transform * vertex.at(2);
    _e1 = vertex.at(1) - vertex.at(0);
}


}// namespace LumenRender