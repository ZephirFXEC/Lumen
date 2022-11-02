//
// Created by enzoc on 14/10/2022.
//

#ifndef LUMEN_MESH_HPP
#define LUMEN_MESH_HPP

#include "../Scene/Object.hpp"
#include "Triangle.hpp"
#include "../../Externals/tiny_obj_loader.h"
#include <iostream>
#include <vector>

namespace LumenRender {

    class Mesh : public Object {
    public:

        explicit Mesh(uint32_t primCount);

        explicit Mesh(const char *file_path);

        bool Hit(Ray &ray, float t_max) const override;

        bool GetBounds(AABB &outbox) const override;

        [[nodiscard]] ObjectType GetType() const override { return ObjectType::MESH; }

    public:
        std::vector<tinyobj::shape_t> m_shapes;
        std::vector<tinyobj::material_t> m_materials;
        std::vector<LumenRender::Triangle *> m_Triangles{};
        std::vector<LumenRender::TriData *> m_TriData{};
        uint32_t m_TriCount = 0;
        glm::vec3 *P = nullptr, *N = nullptr;


    };

} // LumenRender

#endif //LUMEN_MESH_HPP