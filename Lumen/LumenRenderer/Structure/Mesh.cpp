// Copyright (c) 2022.
// Enzo Crema
// All rights reserved

#include "Triangle.hpp"
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

    const bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_path);

    if (!warn.empty()) { std::cout << warn << "\n"; }
    if (!err.empty()) { std::cerr << err << "\n"; }
    if (!ret) { std::cerr << "Failed to load " << file_path << "\n"; }

    for (const auto &shape : shapes) { m_TriCount += shape.mesh.num_face_vertices.size(); }

    ppTriangles = static_cast<Triangle **>(malloc(sizeof(Triangle *) * m_TriCount));
    ppTriData = static_cast<TriData **>(malloc(sizeof(TriData *) * m_TriCount));

    uint32_t triIndex = 0;

    for (auto &shape : shapes) {

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {

            auto fv = shape.mesh.num_face_vertices[f];

            glm::vec3 *tri = nullptr;
            glm::vec3 pos = {};
            glm::vec3 norm = {};
            glm::vec2 uv = {};

            for (size_t v = 0; v < 3; ++v) {
                const tinyobj::index_t idx = shape.mesh.indices[triIndex * fv + v];

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

            ppTriangles[triIndex] = new Triangle(tri);
            ppTriData[triIndex] =
              new TriData({ .N = norm, .UV = uv, .Centroid = (tri[0] + tri[1] + tri[2]) * 0.33334F });

            ppTriangles[triIndex]->pData = ppTriData[triIndex];

            ++triIndex;
        }
    }

    m_BVH = reinterpret_cast<BVH *>(this);
    CalculateBounds(m_Bounds);
}


bool Mesh::Hit(const Ray &ray, float t_max) const
{
    if (m_BVH == nullptr) { return false; }

    if (m_Bounds.IntersectAABB(ray, 1.0F / ray.mDirection, m_Bounds.pMin, m_Bounds.pMax) == 1e30F) { return false; }

    return m_BVH->Hit(ray, t_max);
}


void Mesh::CalculateBounds(AABB &outbox)
{

    for (uint32_t i = 0; i < m_TriCount; ++i) {
        AABB tri_box = AABB();
        ppTriangles[i]->CalculateBounds(tri_box);
        outbox = AABB::Union(outbox, tri_box);
    }
}


Mesh::~Mesh()
{
    delete[] ppTriangles;
    delete[] ppTriData;
    delete m_BVH;
}


}// namespace LumenRender