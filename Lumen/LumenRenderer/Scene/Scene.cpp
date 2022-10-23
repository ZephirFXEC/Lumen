//
// Created by enzoc on 02/10/2022.
//

#include "Scene.hpp"

namespace LumenRender {

    bool Scene::Hit(Ray &ray, float t_max) const {
        Ray temp = ray;
        bool hit_anything = false;
        float closest_so_far = t_max;

        for (const auto &[index, object]: m_Objects) {
            if (object->Hit(temp, t_max) && temp.m_Record.m_T < closest_so_far) {
                hit_anything = true;
                closest_so_far = temp.m_Record.m_T;
                ray = temp;
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
            outbox = first_box ? temp_box : AABB::Union(outbox, temp_box);
            first_box = false;
        }

        return true;
    }

} // LumenRender