//
// Created by enzoc on 02/10/2022.
//

#include "Scene.hpp"

namespace LumenRender {


    auto Scene::Hit(Ray &ray, float t_max) const -> bool {
        Ray temp = ray;
        bool hit_anything = false;
        float closest_so_far = t_max;

        for (const auto &[index, object]: m_Objects) {

            auto hit = std::visit([&](auto&& arg) -> bool {
                return arg->Hit(temp, closest_so_far) && temp.m_Record.m_T < closest_so_far;
            }, object);

            if (hit) {
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
            if (!std::visit([&](auto&& arg) -> bool {
                return arg->GetBounds(temp_box);
            }, object)) {
                return false;
            }

            outbox = first_box ? temp_box : AABB::Union(outbox, temp_box);
            first_box = false;
        }

        return true;
    }

    auto Scene::DeepCopy() const -> std::shared_ptr<IHittable> {
        std::shared_ptr<Scene> const copy = std::make_shared<Scene>();
        for (const auto &[index, object]: m_Objects) {
            std::visit([&](auto&& arg) -> void {
                //copy->AddObject(arg->DeepCopy());
            }, object);
        }
        return copy;
    }


} // LumenRender