//
// Created by enzoc on 03/10/2022.
//

#include "Object.hpp"
#include <glm/gtx/intersect.hpp>

namespace LumenRender {

    bool Sphere::Hit(Ray &ray, float t_max) const {
        float t;
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

    bool Sphere::GetBounds(AABB &outbox) const {
        outbox = { m_Center - glm::vec3(m_Radius), m_Center + glm::vec3(m_Radius) };
        return true;
    }


    bool Plane::Hit(Ray &ray, float t_max) const {
        float t;
        if (!glm::intersectRayPlane(ray.Origin, ray.Direction, m_Center, m_Normal, t)) {
            return false;
        }

        ray.m_Record.m_Position = ray.At(t);
        ray.m_Record.m_Normal = m_Normal;
        ray.m_Record.m_T = t;

        return true;
    }

    bool Plane::GetBounds(AABB &outbox) const {
        outbox = { m_Center - glm::vec3(0.0001f), m_Center + glm::vec3(0.0001f) };
        return true;
    }


} // LumenRender