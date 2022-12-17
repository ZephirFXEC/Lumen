// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#include "Scene.hpp"

namespace LumenRender {


auto Scene::Hit(const Ray &ray, float t_max) const -> bool
{
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