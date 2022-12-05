//
// Created by enzoc on 02/10/2022.
//

#include "Scene.hpp"
#include <algorithm>

namespace LumenRender {


auto Scene::Hit(Ray &ray, float t_max) const -> bool
{
    AABB box = CalculateBounds(box);
    if (box.IntersectAABB(ray, 1.0F / ray.Direction, box.pMin, box.pMax) == 1e30F) { return false; }

    bool hit_anything = false;
    float closest_so_far = t_max;

    for (const auto &[index, object] : m_Objects) {
        if (object->Hit(ray, closest_so_far) && ray.m_Record->m_T < closest_so_far) {
            hit_anything = true;
            closest_so_far = ray.m_Record->m_T;
        }
    }

    return hit_anything;
}

auto Scene::CalculateBounds(AABB &outbox) const -> AABB
{

    for (const auto &[index, object] : m_Objects) { outbox = AABB::Union(object->GetBounds(), outbox); }

    return outbox;
}

auto Scene::DeepCopy() const -> std::shared_ptr<IHittable>
{ return std::make_shared<Scene>(*this); }


}// namespace LumenRender