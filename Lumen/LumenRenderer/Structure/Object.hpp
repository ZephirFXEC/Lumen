//
// Created by enzoc on 06/10/2022.
//

#ifndef LUMEN_OBJECT_HPP
#define LUMEN_OBJECT_HPP

#include <vector>
#include <memory>
#include <cmath>
#include <glm/glm.hpp>

#include "../Ray.hpp"

namespace LumenRender {


    class Object {
    public:
        Object() = default;
        ~Object() = default;

        virtual bool Intersect(const Ray& ray, HitRecords& hit) const = 0;

        uint32_t GetId() const { return m_ObjectID; }


    private:
        uint32_t m_ObjectID{}, m_ShaderID{};
    };

    class TriangleMesh : public Object {
    public:
        TriangleMesh(const uint32_t nfaces,
                     const std::unique_ptr<uint32_t[]> &faceIndex,
                     const std::unique_ptr<uint32_t[]> &vertsIndex,
                     const std::unique_ptr<glm::vec3[]> &verts,
                     std::unique_ptr<glm::vec3[]> &normals,
                     std::unique_ptr<glm::vec2[]> &st);

        bool Intersect(const Ray& ray, HitRecords& hit) const override;


    private:
        uint32_t numTris;                          //number of triangles
        std::unique_ptr<glm::vec3[]> P;               //triangles vertex position
        std::unique_ptr<uint32_t[]> trisIndex;    //vertex index array
        std::unique_ptr<glm::vec3[]> N;               //triangles vertex normals
        std::unique_ptr<glm::vec2[]> texCoordinates;  //triangles texture coordinates
    };

    TriangleMesh* generatePolySphere(float rad, uint32_t divs);





} // LumenRender

#endif //LUMEN_OBJECT_HPP