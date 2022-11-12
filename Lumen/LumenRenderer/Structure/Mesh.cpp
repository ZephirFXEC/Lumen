//
// Created by enzoc on 14/10/2022.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include "Mesh.hpp"
#include "../Accelerators/Bvh.hpp"

namespace LumenRender {

    Mesh::Mesh(const uint32_t& triCount) {
        m_TriCount = triCount;
        m_Triangles.reserve(triCount);
        m_TriData.reserve(triCount);
    }


    Mesh::Mesh(const char *file_path) {
        std::string err;
        std::string warn;
        tinyobj::attrib_t attrib;

        bool ret = tinyobj::LoadObj(&attrib, &m_shapes, &m_materials, &warn, &err, file_path);
        if (!warn.empty()) {
            std::cout << warn << std::endl;
        }
        if (!err.empty()) {
            std::cerr << err << std::endl;
        }
        if (!ret) {
            exit(1);
        }

        m_TriCount = 0;
        for (const auto &shape : m_shapes) {
            m_TriCount += (uint32_t) shape.mesh.num_face_vertices.size();
        }

        m_Triangles.reserve(m_TriCount);
        m_TriData.reserve(m_TriCount);
        uint32_t triIndex = 0;

        for (auto & shape : m_shapes) {

            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                auto fv = size_t(shape.mesh.num_face_vertices.at(f));

                std::vector<TriData> triData{};
                std::vector<glm::vec3> pos{}, norm{};
                std::vector<glm::vec2> uv{};

                for(size_t v = 0; v < fv; v++) {
                    tinyobj::index_t idx = shape.mesh.indices[triIndex * fv + v];

                    pos.emplace_back(attrib.vertices[3 * idx.vertex_index + 0],
                                     attrib.vertices[3 * idx.vertex_index + 1],
                                     attrib.vertices[3 * idx.vertex_index + 2]);

                    norm.emplace_back(attrib.normals[3 * idx.normal_index + 0],
                                      attrib.normals[3 * idx.normal_index + 1],
                                      attrib.normals[3 * idx.normal_index + 2]);

                    uv.emplace_back(attrib.texcoords[2 * idx.texcoord_index + 0],
                                    attrib.texcoords[2 * idx.texcoord_index + 1]);

                }
                pos.clear(); norm.clear(); uv.clear(); triData.clear();

                m_Triangles.emplace_back(new Triangle(pos[0], pos[1], pos[2]));
                m_TriData.emplace_back(new TriData(TriData{.N = norm[0], .UV = uv[0]}));
                triIndex++;
            }
        }

    }

    bool Mesh::Hit(Ray &ray, float t_max) const {
        Ray temp = ray;
        bool hit_tri = false;
        float closest = t_max;

        for (uint32_t i = 0; i < m_TriCount; i++) {
            if (Triangle::TriangleIntersect(temp, m_Triangles.at(i), i) && temp.m_Record.m_T < closest) {
                hit_tri = true;
                closest = temp.m_Record.m_T;
                temp.m_Record.m_Normal = m_TriData.at(i)->N;
                ray = temp;
            }
        }

        return hit_tri;
    }

    bool Mesh::GetBounds(AABB &outbox) const {
        AABB tri_box;
        for (uint32_t i = 0; i < m_TriCount; i++) {
            m_Triangles.at(i)->GetBounds(tri_box);
            outbox = AABB::Union(outbox, tri_box);
        }
        return true;
    }




} // LumenRender