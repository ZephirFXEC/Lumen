//
// Created by enzoc on 14/10/2022.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include "Mesh.hpp"
#include "../Accelerators/Bvh.hpp"
#include <numeric>

namespace LumenRender {

Mesh::Mesh(const char *file_path)
{
    std::string err;
    std::string warn;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    bool const ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_path);

    if (!warn.empty()) { std::cout << warn << "\n"; }
    if (!err.empty()) { std::cerr << err << "\n"; }
    if (!ret) { std::cerr << "Failed to load " << file_path << "\n"; }

    for (const auto &shape : shapes) { m_TriCount += shape.mesh.num_face_vertices.size(); }

    m_Triangles = std::make_unique<Triangle[]>(m_TriCount);
    m_TriData = std::make_unique<TriData[]>(m_TriCount);

    uint32_t triIndex = 0;

    for (auto &shape : shapes) {

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            auto fv = static_cast<size_t>(shape.mesh.num_face_vertices.at(f));

            std::array<glm::vec3, 3> tri{};
            glm::vec3 pos{};
            glm::vec3 norm{};
            glm::vec2 uv{};

            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t const idx = shape.mesh.indices[triIndex * fv + v];


                pos.x = attrib.vertices[3 * idx.vertex_index + 0];
                pos.y = attrib.vertices[3 * idx.vertex_index + 1];
                pos.z = attrib.vertices[3 * idx.vertex_index + 2];

                if (idx.normal_index != -1) {
                    norm.x = attrib.normals[3 * idx.normal_index + 0];
                    norm.y = attrib.normals[3 * idx.normal_index + 1];
                    norm.z = attrib.normals[3 * idx.normal_index + 2];
                }

                if (idx.texcoord_index != -1) {
                    uv.x = attrib.texcoords[2 * idx.texcoord_index + 0];
                    uv.y = attrib.texcoords[2 * idx.texcoord_index + 1];
                }

                tri.at(v) = pos;
            }
            m_Triangles[triIndex] = Triangle(tri);
            m_TriData[triIndex] =
              TriData({ .N = norm, .UV = uv, .Centroid = (tri.at(0) + tri.at(1) + tri.at(2)) * 0.3334F });

            m_Triangles[triIndex].m_Data = &m_TriData[triIndex];

            triIndex++;
        }
    }

    m_BVH = std::make_unique<BVH>(this);
    m_Bounds = CalculateBounds(m_Bounds);
    CalculateBounds(m_Bounds);
}


auto Mesh::Hit(Ray &ray, float t_max) const -> bool
{
    if (m_BVH != nullptr) { return m_BVH->Hit(ray, t_max); }

    return false;
}


auto Mesh::CalculateBounds(AABB &outbox) const -> AABB
{

    for (uint32_t i = 0; i < m_TriCount; i++) {
        AABB tri_box = AABB();
        m_Triangles[i].CalculateBounds(tri_box);
        outbox = AABB::Union(outbox, tri_box);
    }
    return outbox;
}


Mesh::~Mesh()
{
    m_Triangles.reset();
    m_TriData.reset();
    m_BVH.reset();
}


}// namespace LumenRender