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

    //TODO: get rid of this class
    class Object {
    public:
        virtual ~Object() = default;

        virtual auto Hit(Ray &ray, float t_max) const -> bool = 0;

        virtual auto GetBounds(AABB &outbox) const -> bool = 0;
    };

    class Sphere : public Object {
    public:
        Sphere() = default;

        Sphere(const glm::vec3 &center, float radius) : m_Center(center), m_Radius(radius) {}

        auto Hit(Ray &ray, float t_max) const -> bool override;

        auto GetBounds(AABB &outbox) const -> bool override;


        glm::vec3 m_Center{};
        float m_Radius{};
    };


    class Plane : public Object {
    public:
        Plane() = default;

        Plane(const glm::vec3 &center, const glm::vec3 &normal) : m_Center(center), m_Normal(normal) {}

        auto Hit(Ray &ray, float t_max) const -> bool override;

        auto GetBounds(AABB &outbox) const -> bool override;


        glm::vec3 m_Center{};
        glm::vec3 m_Normal{};
    };
} // LumenRender

#endif //LUMEN_OBJECT_HPP