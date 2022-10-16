//
// Created by enzoc on 14/10/2022.
//

#ifndef LUMEN_TRIANGLE_MESH_HPP
#define LUMEN_TRIANGLE_MESH_HPP

#include "../Scene/Object.hpp"
#include "Triangle.hpp"
#include "../../Externals/tiny_obj_loader.h"
#include <iostream>
#include <vector>

namespace LumenRender {

    class Triangle_Mesh : public Object {
    public:
        explicit Triangle_Mesh(const char *file_path);

        bool Hit(const Ray &ray, HitRecords &record) const override;

        bool GetBounds(AABB &outbox) const override;

    public:
        std::vector<tinyobj::shape_t> m_shapes;
        std::vector<tinyobj::material_t> m_materials;
        std::vector<std::shared_ptr<LumenRender::Triangle>> m_Triangles;
    };

} // LumenRender

#endif //LUMEN_TRIANGLE_MESH_HPP