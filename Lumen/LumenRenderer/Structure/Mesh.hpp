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
    ~Mesh() override;

    auto Hit(const Ray &ray, float t_max) const -> bool;

    auto CalculateBounds(AABB &outbox) const -> AABB;

    [[nodiscard]] auto DeepCopy() const -> std::shared_ptr<IHittable>;

    [[nodiscard]] __forceinline auto GetBounds() const & -> AABB { return m_Bounds; }
    [[nodiscard]] __forceinline auto GetTriangles() const & -> const std::unique_ptr<LumenRender::Triangle[]> &
    {
        return m_Triangles;
    }
    [[nodiscard]] __forceinline auto GetTriData() const & -> const std::unique_ptr<TriData[]> & { return m_TriData; }
    [[nodiscard]] __forceinline auto GetTriCount() const & -> size_t { return m_TriCount; }

  private:
    AABB m_Bounds{};
    size_t m_TriCount{};

    std::unique_ptr<LumenRender::Triangle[]> m_Triangles;
    std::unique_ptr<LumenRender::TriData[]> m_TriData;
    std::unique_ptr<class BVH> m_BVH;
};

}// namespace LumenRender

#endif// LUMEN_MESH_HPP