// Copyright (c) 2022.
// Enzo Crema
// All rights reserved

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

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {

            auto fv = shape.mesh.num_face_vertices[f];

            std::array<glm::vec3, 3> tri{};
            glm::vec3 pos{};
            glm::vec3 norm{};
            glm::vec2 uv{};

            for (size_t v = 0; v < 3; ++v) {
                tinyobj::index_t const idx = shape.mesh.indices[triIndex * fv + v];


                pos.x = attrib.vertices[3 * static_cast<uint64_t>(idx.vertex_index) + 0];
                pos.y = attrib.vertices[3 * static_cast<uint64_t>(idx.vertex_index) + 1];
                pos.z = attrib.vertices[3 * static_cast<uint64_t>(idx.vertex_index) + 2];

                if (idx.normal_index != -1) {
                    norm.x = attrib.normals[3 * static_cast<uint64_t>(idx.normal_index) + 0];
                    norm.y = attrib.normals[3 * static_cast<uint64_t>(idx.normal_index) + 1];
                    norm.z = attrib.normals[3 * static_cast<uint64_t>(idx.normal_index) + 2];
                }

                if (idx.texcoord_index != -1) {
                    uv.x = attrib.texcoords[2 * static_cast<uint64_t>(idx.texcoord_index) + 0];
                    uv.y = attrib.texcoords[2 * static_cast<uint64_t>(idx.texcoord_index) + 1];
                }

                tri[v] = pos;
            }
            m_Triangles[triIndex] = Triangle(tri);
            m_TriData[triIndex] = TriData({ .N = norm, .UV = uv, .Centroid = (tri[0] + tri[1] + tri[2]) * 0.33334F });

            m_Triangles[triIndex].m_Data = &m_TriData[triIndex];

            ++triIndex;
        }
    }

    m_BVH = std::make_unique<BVH>(this);
    m_Bounds = CalculateBounds(m_Bounds);
    CalculateBounds(m_Bounds);
}


auto Mesh::Hit(const Ray &ray, float t_max) const -> bool
{
    if (m_BVH == nullptr) { return false; }

    if (m_Bounds.IntersectAABB(ray, 1.0F / ray.Direction, m_Bounds.pMin, m_Bounds.pMax) == 1e30F) { return false; }

    return m_BVH->Hit(ray, t_max);
}


auto Mesh::CalculateBounds(AABB &outbox) const -> AABB
{

    for (uint32_t i = 0; i < m_TriCount; ++i) {
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
auto Mesh::DeepCopy() const -> std::shared_ptr<IHittable> { return IHittable::DeepCopy(); }


}// namespace LumenRender