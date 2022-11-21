//
// Created by enzoc on 02/10/2022.
//

#include "Scene.hpp"

namespace LumenRender {


auto Scene::Hit(Ray &ray, float t_max) const -> bool
{
    if (m_Objects.empty()) { return false; }


    bool hit_anything = false;
    float closest_so_far = t_max;


    // Check if AABB intersect with ray
    AABB box = CalculateBounds(box);
    if (box.IntersectAABB(ray, box.pMin, box.pMax) == 1e30F) { return false; }

    // If AABB intersect, check if objects intersect
    for (const auto &[index, object] : m_Objects) {

        if (object->Hit(ray, closest_so_far) && ray.m_Record.m_T < closest_so_far) {
            hit_anything = true;
            closest_so_far = ray.m_Record.m_T;
        }
    }

    return hit_anything;
}

auto Scene::CalculateBounds(AABB &outbox) const -> AABB
{

    for (const auto &[index, object] : m_Objects) { outbox = AABB::Union(object->GetBounds(), outbox); }

    /*
    for (const auto &[index, object]: m_Objects) {
        auto box = std::visit([&](auto&& arg) -> AABB {
            return arg->GetBounds(temp_box);
        }, object);

        outbox = first_box ? box : AABB::Union(outbox, box);
        first_box = false;
    } */


    return outbox;
}

auto Scene::DeepCopy() const -> std::shared_ptr<IHittable>
{
    auto scene = std::make_shared<Scene>();
    /*
            for (const auto &[index, object]: m_Objects) {
                scene->AddObject(object);
            }
    */
    return scene;
}


}// namespace LumenRender