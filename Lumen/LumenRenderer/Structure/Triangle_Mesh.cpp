//
// Created by enzoc on 14/10/2022.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include "Triangle_Mesh.hpp"


namespace LumenRender {

    Triangle_Mesh::Triangle_Mesh(const char *file_path) {
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

        std::vector<Vertex> vertices;

// Loop over shapes
        for (const auto &shape: shapes) {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {

                auto fv = size_t(shape.mesh.num_face_vertices[f]);

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    Vertex vert{};

                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                    vert.P = { vx, vy, vz };


                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                        vert.N = { nx, ny, nz };
                    }

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                        vert.UV = { tx, ty };
                    }

                    vertices.push_back(vert);
                }
                index_offset += fv;
            }
        }



        // Loops vertices
        for (int i = 0; i < vertices.size(); i += 3) {
            m_Triangles.push_back(new LumenRender::Triangle(vertices[i], vertices[i + 1], vertices[i + 2]));
        }

        std::cout << "> Successfully opened " << inputfile << "! \n\n";
    }

    bool Triangle_Mesh::Hit(const Ray &ray, float t_max, HitRecords &record) const {
        HitRecords temp{};
        bool hit_tri = false;
        float closest = t_max;

        for (auto &tri: m_Triangles) {
            if (tri->Hit(ray, t_max, temp) && temp.m_T < closest) {
                hit_tri = true;
                closest = temp.m_T;
                record = temp;
            }
        }
        return hit_tri;
    }

    bool Triangle_Mesh::GetBounds(AABB &outbox) const {
        AABB tri_box;
        for (auto &tri: m_Triangles) {
            tri->GetBounds(tri_box);
            outbox = AABB::Union(outbox, tri_box);
        }
        return true;
    }

} // LumenRender