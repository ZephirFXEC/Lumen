//
// Created by enzoc on 02/10/2022.
//

#include "Scene.hpp"

namespace LumenRender {

    void Scene::AddObject(IHittable *object) {
        m_Objects.insert({ m_Index, object });
        m_Index++;
    }

    auto Scene::Hit(Ray &ray, float t_max) const -> bool {
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

    auto Scene::GetBounds(AABB &outbox) const -> bool {
        if (m_Objects.empty()) {
            return false;
        }

        AABB temp_box;
        bool first_box = true;

        for (const auto &[index, object]: m_Objects) {
            if (!object->GetBounds(temp_box)) {
                return false;
            }
            outbox = first_box ? temp_box : AABB::Union(outbox, temp_box);
            first_box = false;
        }

        return true;
    }

    auto Scene::DeepCopy() const -> std::shared_ptr<IHittable> {
        std::shared_ptr<Scene> copy = std::make_shared<Scene>();
        for (const auto &[index, object]: m_Objects) {
            copy->AddObject(object->DeepCopy().get()); //TODO: object DeepCopy
        }
        return copy;
    }


} // LumenRender