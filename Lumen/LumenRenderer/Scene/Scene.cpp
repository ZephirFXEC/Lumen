//
// Created by enzoc on 02/10/2022.
//

#include "Scene.hpp"

namespace LumenRender {


    Scene *Scene::m_Instance{ nullptr };
    std::mutex Scene::m_Mutex;

    Scene *Scene::GetInstance() {

        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Instance == nullptr) {
            m_Instance = new Scene();
        }
        return m_Instance;
    }

    void Scene::AddObject(Object *object) {
        m_Objects.insert({ m_Index, object });
        m_Index++;
    }

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