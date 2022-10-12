//
// Created by enzoc on 03/10/2022.
//

#include "Object.hpp"
#include <glm/gtx/intersect.hpp>

namespace LumenRender {

    bool Sphere::Hit(const Ray &ray, HitRecords &record) const {
        float t;
        if (glm::intersectRaySphere(ray.Origin, ray.Direction, m_Center, m_Radius * m_Radius, t)) {
            record.m_Position = ray.At(t);
            record.m_Normal = glm::normalize(record.m_Position - m_Center);
            record.m_T = t;

            return true;
        }
        return false;
    }


    bool Plane::Hit(const Ray &ray, HitRecords &record) const {
        float t;
        if (glm::intersectRayPlane(ray.Origin, ray.Direction, m_Center, m_Normal, t)) {
            record.m_Position = ray.At(t);
            record.m_Normal = m_Normal;
            record.m_T = t;

            return true;
        }
        return false;
    }


} // LumenRender