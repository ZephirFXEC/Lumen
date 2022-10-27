//
// Created by enzoc on 03/10/2022.
//

#ifndef LUMEN_OBJECT_HPP
#define LUMEN_OBJECT_HPP

#include <string>
#include <glm/glm.hpp>
#include "../Ray.hpp"
#include "../Accelerators/Aabb.hpp"


namespace LumenRender {
    enum class ObjectType {
        SPHERE,
        MESH,
        TRIANGLE,
        PLANE,
        SCENE
    };

    class Object {
    public:
        virtual ~Object() = default;
        virtual bool Hit(Ray &ray, float t_max) const = 0;
        virtual bool GetBounds(AABB &outbox) const = 0;
        virtual ObjectType GetType() const = 0;
    };

    class Sphere : public Object {
    public:
        Sphere() = default;

        Sphere(const glm::vec3 &center, float radius) : m_Center(center), m_Radius(radius) {}

        bool Hit(Ray &ray, float t_max) const override;
        bool GetBounds(AABB &outbox) const override;
        ObjectType GetType() const override { return ObjectType::SPHERE; }

    public:
        glm::vec3 m_Center{};
        float m_Radius{};
    };


    class Plane : public Object {
    public:
        Plane() = default;

        Plane(const glm::vec3 &center, const glm::vec3 &normal) : m_Center(center), m_Normal(normal) {}

        bool Hit(Ray &ray, float t_max) const override;
        bool GetBounds(AABB &outbox) const override;
        ObjectType GetType() const override { return ObjectType::PLANE; }

    public:
        glm::vec3 m_Center{};
        glm::vec3 m_Normal{};
    };
} // LumenRender

#endif //LUMEN_OBJECT_HPP