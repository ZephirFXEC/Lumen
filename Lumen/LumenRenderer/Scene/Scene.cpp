// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#include "Scene.hpp"
#include "glm/fwd.hpp"

namespace LumenRender {


bool Scene::Hit(const Ray &ray, float t_max) const
{
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (uint32_t i = 0; i < m_Index; i++) {
        if (mObjects[i].Hit(ray, closest_so_far) && ray.pRecord->mT < closest_so_far) {
            hit_anything = true;
            closest_so_far = ray.pRecord->mT;
            ray.pRecord->mType = Type::HIT;
        }
    }

    return hit_anything;
}

void Scene::CalculateBounds(AABB &outbox) const
{

    for (uint32_t i = 0; i < m_Index; i++) { outbox = AABB::Union(mObjects[i].GetBounds(), outbox); }
}

void Scene::AddObject(IHittable<Mesh> *object)
{
    mObjects[m_Index] = *object;
    m_Index++;
};


}// namespace LumenRender