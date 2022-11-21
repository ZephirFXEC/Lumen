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

    auto Hit(Ray &ray, float t_max) const -> bool;

    auto CalculateBounds(AABB &outbox) const -> AABB;

    auto Transform(const glm::mat3 &transform) const -> void;

    [[nodiscard]] auto DeepCopy() const -> std::shared_ptr<IHittable>;

    auto GetBounds() const & -> AABB { return m_Bounds; }

  private:
    std::vector<tinyobj::shape_t> m_shapes;
    std::vector<tinyobj::material_t> m_materials;

  public:
    LumenRender::Triangle *m_Triangles = nullptr;
    LumenRender::TriData *m_TriData = nullptr;
    class BVH *m_BVH = nullptr;
    uint32_t m_TriCount = 0;
    AABB m_Bounds;
};

}// namespace LumenRender

#endif// LUMEN_MESH_HPP