//
// Created by enzoc on 14/10/2022.
//

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

    auto Hit(Ray &ray, float t_max) const -> bool;

    auto CalculateBounds(AABB &outbox) const -> AABB;


    [[nodiscard]] auto DeepCopy() const -> std::shared_ptr<IHittable>;

    [[nodiscard]] __forceinline auto GetBounds() const & -> AABB { return m_Bounds; }
    [[nodiscard]] __forceinline auto GetTriangles() const & -> Triangle * { return m_Triangles; }
    [[nodiscard]] __forceinline auto GetTriData() const & -> TriData * { return m_TriData; }
    [[nodiscard]] __forceinline auto GetTriCount() const & -> uint32_t { return m_TriCount; }

  private:
    LumenRender::Triangle *m_Triangles{ nullptr };
    LumenRender::TriData *m_TriData{ nullptr };
    class BVH *m_BVH{ nullptr };

    AABB m_Bounds{};
    uint32_t m_TriCount{ 0 };
};

}// namespace LumenRender

#endif// LUMEN_MESH_HPP