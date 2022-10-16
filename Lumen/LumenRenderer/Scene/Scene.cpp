//
// Created by enzoc on 02/10/2022.
//

#include "Scene.hpp"

namespace LumenRender {

    bool Scene::Hit(const Ray &ray, HitRecords &record) const {
        HitRecords temp_rec{};
        bool hit_anything = false;
        float closest_so_far = std::numeric_limits<float>::max();

        for (const auto &[index, object]: m_Objects) {
            if (object->Hit(ray, temp_rec) && temp_rec.m_T < closest_so_far) {
                hit_anything = true;
                closest_so_far = temp_rec.m_T;
                record = temp_rec;
            }
        }

        return hit_anything;
    }

    bool Scene::GetBounds(AABB &outbox) const {
        if (m_Objects.empty()) return false;

        AABB temp_box;
        bool first_box = true;

        for (const auto &[index, object]: m_Objects) {
            if (!object->GetBounds(temp_box)) return false;
            outbox = first_box ? temp_box : AABB::SurroundingBox(outbox, temp_box);
            first_box = false;
        }

        return true;
    }

} // LumenRender