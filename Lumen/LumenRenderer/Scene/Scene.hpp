// Copyright (c) 2022.
// Enzo Crema
// All rights reserved

#ifndef LUMEN_SCENE_HPP
#define LUMEN_SCENE_HPP

#include "../Accelerators/Aabb.hpp"
#include "../Accelerators/Bvh.hpp"
#include "../Ray.hpp"
#include "../Structure/Mesh.hpp"
#include "../Structure/Triangle.hpp"

#include <unordered_map>

namespace LumenRender {

class Scene : public IHittable<Scene>
{
  public:
    Scene() = default;

    void AddObject(IHittable<Mesh> *object);
    void CalculateBounds(AABB &outbox) const;
    bool Hit(const Ray &ray, float t_max) const;

    IHittable<Mesh> *mObjects{};
    uint32_t m_Index = 0;

  private:
};


}// namespace LumenRender

#endif// LUMEN_SCENE_HPP