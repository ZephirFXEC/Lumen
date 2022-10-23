//
// Created by enzoc on 14/10/2022.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include "Mesh.hpp"


namespace LumenRender {


    Mesh::Mesh(const uint32_t primCount) {
        m_Triangles = (LumenRender::Triangle *) _aligned_malloc(primCount * sizeof(LumenRender::Triangle), 128);
        memset((void *) m_Triangles, 0, primCount * sizeof(LumenRender::Triangle));
        m_TriData = (LumenRender::TriData *) _aligned_malloc(primCount * sizeof(LumenRender::TriData), 64);
        memset(m_TriData, 0, primCount * sizeof(LumenRender::TriData));
        m_TriCount = primCount;
    }


    Mesh::Mesh(const char *file_path) {
        std::string inputfile = file_path;
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "./"; // Path to material files

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(inputfile, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            exit(1);
        }

        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto &attrib = reader.GetAttrib();
        auto &shapes = reader.GetShapes();
        auto &materials = reader.GetMaterials();


// Loop over shapes
        for (const auto &shape: shapes) {

            m_Triangles = new LumenRender::Triangle[shape.mesh.indices.size() / 3];
            m_TriData = new LumenRender::TriData[shape.mesh.indices.size() / 3];
            P = new glm::vec3[shape.mesh.indices.size()];
            N = new glm::vec3[shape.mesh.indices.size()];
            auto *uv = new glm::vec2[shape.mesh.indices.size()];

            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {

                auto fv = size_t(shape.mesh.num_face_vertices[f]);

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    Triangle vert{};

                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                    P[v] = { vx, vy, vz };
                    m_Triangles[f].vertex0 = P[0];
                    m_Triangles[f].vertex1 = P[1];
                    m_Triangles[f].vertex2 = P[2];



                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                        N[v] = { nx, ny, nz };
                        m_TriData[f].N[0] = N[0];
                        m_TriData[f].N[1] = N[1];
                        m_TriData[f].N[2] = N[2];
                    }

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                        uv[v] = { tx, ty };
                    }

                }
                index_offset += fv;
            }
        }

        //TODO: build bvh here


        std::cout << "> Successfully opened " << inputfile << "! \n\n";
    }

    bool Mesh::Hit(Ray &ray, float t_max) const {
        HitRecords temp{};
        bool hit_tri = false;
        float closest = t_max;

        for (uint32_t i = 0; i < m_TriCount; i++) {
            if (m_Triangles[i].Hit(ray, t_max) && temp.m_T < closest) {
                hit_tri = true;
                closest = temp.m_T;
                ray.m_Record = temp;
            }
        }
        return hit_tri;
    }

    bool Mesh::GetBounds(AABB &outbox) const {
        AABB tri_box;
        for (uint32_t i = 0; i < m_TriCount; i++) {
            m_Triangles[i].GetBounds(tri_box);
            outbox = AABB::Union(outbox, tri_box);
        }
        return true;
    }


} // LumenRender