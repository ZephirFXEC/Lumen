//
// Created by enzoc on 06/10/2022.
//

#ifndef LUMEN_OBJECT_HPP
#define LUMEN_OBJECT_HPP

#include <cstdint>
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


    class Triangle : public Object {
    public:
        Triangle() = default;
        Triangle(const glm::vec3& vert0, const glm::vec3& vert1, const glm::vec3& vert2)
            : vert0(vert0), vert1(vert1), vert2(vert2) {}

        bool Intersect(const Ray& ray, HitRecords& hit) const override;

    private:
        glm::vec3 vert0{}, vert1{}, vert2{};
    };




} // LumenRender

#endif //LUMEN_OBJECT_HPP