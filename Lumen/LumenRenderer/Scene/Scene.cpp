//
// Created by enzoc on 02/10/2022.
//

#include "Scene.hpp"

namespace LumenRender {

    bool Scene::Hit(const Ray &ray, HitRecords &record) {
        HitRecords tempRecord{};
        bool hitAnything = false;
        float closestSoFar = record.m_T;

        for (const auto& object : m_Objects) {
            if (object->Hit(ray, tempRecord) && tempRecord.m_T < closestSoFar) {
                hitAnything = true;
                closestSoFar = tempRecord.m_T;
                record = tempRecord;
            }
        }

        return hitAnything;
    }
} // LumenRender