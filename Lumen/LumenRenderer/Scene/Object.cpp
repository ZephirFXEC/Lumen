//
// Created by enzoc on 03/10/2022.
//

#include "Object.hpp"

#include <cmath>
#include <glm/gtx/intersect.hpp>

namespace LumenRender {

    auto Sphere::Hit(Ray &ray, float t_max) const -> bool {
        float t = NAN;
        if (glm::intersectRaySphere(ray.Origin, ray.Direction, m_Center, m_Radius * m_Radius, t)) {
            if (t < t_max) {
                ray.m_Record.m_T = t;
                ray.m_Record.m_Position = ray.At(t);
                ray.m_Record.m_Normal = glm::normalize(ray.m_Record.m_Position - m_Center);
                return true;
            }
        }
        return false;
    }

    auto Sphere::GetBounds(AABB &outbox) const -> bool {
        outbox = { m_Center - glm::vec3(m_Radius), m_Center + glm::vec3(m_Radius) };
        return true;
    }


    auto Plane::Hit(Ray &ray, float t_max) const -> bool {
        float t = NAN;
        if (!glm::intersectRayPlane(ray.Origin, ray.Direction, m_Center, m_Normal, t) || t < t_max) {
            return false;
        }

        ray.m_Record.m_Position = ray.At(t);
        ray.m_Record.m_Normal = m_Normal;
        ray.m_Record.m_T = t;

        return true;
    }

    auto Plane::GetBounds(AABB &outbox) const -> bool {
        outbox = { m_Center - glm::vec3(0.0001F), m_Center + glm::vec3(0.0001F) };
        return true;
    }


} // LumenRender