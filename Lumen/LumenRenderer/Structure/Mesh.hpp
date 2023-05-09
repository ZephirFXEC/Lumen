// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#ifndef LUMEN_MESH_HPP
#define LUMEN_MESH_HPP

#include "../../Externals/tiny_obj_loader.h"
#include "../Interfaces/IHittable.hpp"
#include "Triangle.hpp"
#include <iostream>
#include <vector>


namespace LumenRender {

class Mesh : public IHittable<Mesh>
{
  public:
    explicit Mesh(const char *file_path);
    ~Mesh();
    void CalculateBounds(AABB &outbox);


    [[nodiscard]] bool Hit(const Ray &ray, float t_max) const;

    [[nodiscard]] inline const AABB &GetBounds() const { return m_Bounds; }
    [[nodiscard]] inline Triangle **GetTriangles() const { return ppTriangles; }
    [[nodiscard]] inline TriData **GetTriData() const { return ppTriData; }
    [[nodiscard]] inline const size_t &GetTriCount() const { return m_TriCount; }

  private:
    AABB m_Bounds{};
    size_t m_TriCount{};

    Triangle **ppTriangles;
    TriData **ppTriData;
    class BVH *m_BVH;
};

}// namespace LumenRender

#endif// LUMEN_MESH_HPP