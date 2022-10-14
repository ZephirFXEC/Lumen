//
// Created by enzoc on 03/10/2022.
//

#ifndef LUMEN_OBJECT_HPP
#define LUMEN_OBJECT_HPP

#include <glm/glm.hpp>
#include "../Ray.hpp"
#include <string>

namespace LumenRender {
    class Object {
    public:
        Object() = default;
        virtual ~Object() = default;

        enum Types {
            Scene,
            Sphere,
            Plane,
            Triangle,
            Mesh,
            Light
        };

        virtual bool Hit(const Ray& ray, HitRecords& record) const = 0;

        uint32_t m_Index{};
    };

    class Sphere : public Object {
    public:
        Sphere() = default;
        Sphere(const glm::vec3& center, float radius) : m_Center(center), m_Radius(radius) {}

        bool Hit(const Ray& ray, HitRecords& record) const override;

    public:
        glm::vec3 m_Center{};
        float m_Radius{};
    };



    class Plane : public Object {
    public:
        Plane() = default;
        Plane(const glm::vec3& center, const glm::vec3& normal) : m_Center(center), m_Normal(normal) {}

        bool Hit(const Ray& ray, HitRecords& record) const override;

    public:
        glm::vec3 m_Center{};
        glm::vec3 m_Normal{};
    };
} // LumenRender

#endif //LUMEN_OBJECT_HPP