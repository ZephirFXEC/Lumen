//
// Created by enzoc on 14/10/2022.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include "Mesh.hpp"
#include "../Accelerators/Bvh.hpp"

namespace LumenRender {

    Mesh::Mesh(const char *file_path) {
        std::string err;
        std::string warn;
        tinyobj::attrib_t attrib;

        bool const ret = tinyobj::LoadObj(&attrib, &m_shapes, &m_materials, &warn, &err, file_path);
        if (!warn.empty()) {
            std::cout << warn << std::endl;
        }
        if (!err.empty()) {
            std::cerr << err << std::endl;
        }
        if (!ret) {
            exit(1);
        }


        for (const auto &shape : m_shapes) {
            m_TriCount += static_cast<uint32_t>(shape.mesh.num_face_vertices.size());
        }

        m_Triangles = new Triangle[m_TriCount];
        m_TriData = new TriData[m_TriCount];
        uint32_t triIndex = 0;

        for (auto & shape : m_shapes) {

            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                auto fv = static_cast<size_t>(shape.mesh.num_face_vertices.at(f));

                std::array<glm::vec3, 3> tri{};
                glm::vec3 pos{};
                glm::vec3 norm{};
                glm::vec2 uv{};

                for(size_t v = 0; v < 3; v++) {
                    tinyobj::index_t const idx = shape.mesh.indices[triIndex * fv + v];



                    pos.x = attrib.vertices[3 * idx.vertex_index + 0];
                    pos.y = attrib.vertices[3 * idx.vertex_index + 1];
                    pos.z = attrib.vertices[3 * idx.vertex_index + 2];

                    if(idx.normal_index != -1) {
                        norm.x = attrib.normals[3 * idx.normal_index + 0];
                        norm.y = attrib.normals[3 * idx.normal_index + 1];
                        norm.z = attrib.normals[3 * idx.normal_index + 2];
                    }

                    if(idx.texcoord_index != -1) {
                        uv.x = attrib.texcoords[2 * idx.texcoord_index + 0];
                        uv.y = attrib.texcoords[2 * idx.texcoord_index + 1];
                    }

                    tri.at(v) = pos;
                }
                m_Triangles[triIndex] = Triangle(tri);
                m_TriData[triIndex] = TriData({.N = norm, .UV = uv});
                triIndex++;
            }

        }

    }

    auto Mesh::Hit(Ray &ray, float t_max) const -> bool {
        Ray temp = ray;
        bool hit_tri = false;
        float closest = t_max;

        for (uint32_t i = 0; i < m_TriCount; i++) {
            if (Triangle::TriangleIntersect(temp, m_Triangles[i], i) && temp.m_Record.m_T < closest) {
                hit_tri = true;
                closest = temp.m_Record.m_T;
                temp.m_Record.m_Normal = m_TriData[i].N;
                ray = temp;
            }
        }

        return hit_tri;
    }

    auto Mesh::GetBounds(AABB &outbox) const -> AABB {
        AABB tri_box;
        for (uint32_t i = 0; i < m_TriCount; i++) {
            m_Triangles[i].GetBounds(tri_box);
            outbox = AABB::Union(outbox, tri_box);
        }
        return outbox;
    }

    auto Mesh::DeepCopy() const -> std::shared_ptr<IHittable> {
        return std::make_shared<Mesh>(*this);
    }


} // LumenRender