//
// Created by enzoc on 14/10/2022.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include "Mesh.hpp"


namespace LumenRender {


    Mesh::Mesh(const uint32_t primCount) {
        /*
        m_Triangles = (LumenRender::Triangle *) _aligned_malloc(primCount * sizeof(LumenRender::Triangle), 128);
        memset((void *) m_Triangles, 0, primCount * sizeof(LumenRender::Triangle));
        m_TriData = (LumenRender::TriData *) _aligned_malloc(primCount * sizeof(LumenRender::TriData), 64);
        memset(&m_TriData, 0, primCount * sizeof(LumenRender::TriData));
        m_TriCount = primCount;
        */
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
        for (const auto &shape: m_shapes) {
            m_TriCount += static_cast<uint32_t>(shape.mesh.num_face_vertices.size());
        }

        m_Triangles.reserve(m_TriCount);
        m_TriData.reserve(m_TriCount);

        uint32_t triIndex = 0;
        for (const auto &shape: m_shapes) {
            for (const auto &index: shape.mesh.indices) {
                Triangle tri;
                TriData triData{};

                tri.vertex0 = glm::vec3(attrib.vertices[3 * index.vertex_index + 0],
                                        attrib.vertices[3 * index.vertex_index + 1],
                                        attrib.vertices[3 * index.vertex_index + 2]);

                tri.vertex1 = glm::vec3(attrib.vertices[3 * index.vertex_index + 3],
                                        attrib.vertices[3 * index.vertex_index + 4],
                                        attrib.vertices[3 * index.vertex_index + 5]);

                tri.vertex2 = glm::vec3(attrib.vertices[3 * index.vertex_index + 6],
                                        attrib.vertices[3 * index.vertex_index + 7],
                                        attrib.vertices[3 * index.vertex_index + 8]);

                tri.centroid = (tri.vertex0 + tri.vertex1 + tri.vertex2) / 3.0f;


                if(index.normal_index >= 0) {
                    triData.N[0] = glm::vec3(attrib.normals[3 * index.normal_index + 0],
                                             attrib.normals[3 * index.normal_index + 1],
                                             attrib.normals[3 * index.normal_index + 2]);

                    triData.N[1] = glm::vec3(attrib.normals[3 * index.normal_index + 3],
                                             attrib.normals[3 * index.normal_index + 4],
                                             attrib.normals[3 * index.normal_index + 5]);

                    triData.N[2] = glm::vec3(attrib.normals[3 * index.normal_index + 6],
                                             attrib.normals[3 * index.normal_index + 7],
                                             attrib.normals[3 * index.normal_index + 8]);
                }

                if(index.texcoord_index >= 0) {
                    triData.UV[0] = glm::vec2(attrib.texcoords[2 * index.texcoord_index + 0],
                                              attrib.texcoords[2 * index.texcoord_index + 1]);

                    triData.UV[1] = glm::vec2(attrib.texcoords[2 * index.texcoord_index + 2],
                                              attrib.texcoords[2 * index.texcoord_index + 3]);
                }

                m_Triangles.push_back(new Triangle(tri));
                m_TriData.push_back(new TriData(triData));
            }
        }
    }

    bool Mesh::Hit(Ray &ray, float t_max) const {
        Ray temp = ray;
        bool hit_tri = false;
        float closest = t_max;

        for (uint32_t i = 0; i < m_TriCount; i++) {
            if (m_Triangles.at(i)->Hit(temp, t_max) && temp.m_Record.m_T < closest) {
                hit_tri = true;
                closest = temp.m_Record.m_T;
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