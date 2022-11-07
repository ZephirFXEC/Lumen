//
// Created by enzoc on 14/10/2022.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include "Mesh.hpp"


namespace LumenRender {


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
        for (const auto &shape: m_shapes) {
            m_TriCount += static_cast<uint32_t>(shape.mesh.num_face_vertices.size());
        }

        m_Triangles.reserve(m_TriCount);
        m_TriData.reserve(m_TriCount);

        uint32_t triIndex = 0;
        for (auto & shape : m_shapes) {
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                auto fv = size_t(shape.mesh.num_face_vertices.at(f));
                TriData triData{};
                std::vector<glm::vec3> pos, norm;
                std::vector<glm::vec3> uv;
                for(size_t v = 0; v < fv; v++) {
                    tinyobj::index_t idx = shape.mesh.indices[triIndex * fv + v];

                    pos.reserve(3); norm.reserve(3); uv.reserve(3);
                    pos.emplace_back(attrib.vertices[3 * idx.vertex_index + 0],
                                     attrib.vertices[3 * idx.vertex_index + 1],
                                     attrib.vertices[3 * idx.vertex_index + 2]);

                    norm.emplace_back(attrib.normals[3 * idx.normal_index + 0],
                                      attrib.normals[3 * idx.normal_index + 1],
                                      attrib.normals[3 * idx.normal_index + 2]);

                    uv.emplace_back(attrib.texcoords[2 * idx.texcoord_index + 0],
                                    attrib.texcoords[2 * idx.texcoord_index + 1],
                                    0.0f);

                    uv[v].z = 1.0f - (uv[v].x + uv[v].y);

                    //TODO: Fix this
                    triData.N[0] = norm[0];
                    triData.N[1] = norm[1];
                    triData.N[2] = norm[2];
                    triData.UVW[0] = uv[0];
                    triData.UVW[1] = uv[1];
                    triData.UVW[2] = uv[2];

                }
                pos.clear(); norm.clear(); uv.clear();

                m_Triangles.push_back(new Triangle(pos[0], pos[1], pos[2]));
                m_TriData.push_back(new TriData(triData));
                triIndex++;
            }
        }

    }

    bool Mesh::Hit(Ray &ray, float t_max) const {
        Ray temp = ray;
        bool hit_tri = false;
        float closest = t_max;
#if 1
        for (uint32_t i = 0; i < m_TriCount; i++) {
            if (m_Triangles.at(i)->Hit(temp, t_max) && temp.m_Record.m_T < closest) {
                hit_tri = true;
                closest = temp.m_Record.m_T;
                ray = temp;
                //TODO: read data from m_TriData
            }
        }
#else
        if(m_BVH->Hit(temp, t_max) && temp.m_Record.m_T < closest) {
            hit_tri = true;
            closest = temp.m_Record.m_T;
            ray = temp;
        }
#endif

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