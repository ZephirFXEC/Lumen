//
// Created by enzoc on 03/10/2022.
//

#ifndef LUMEN_OBJECT_HPP
#define LUMEN_OBJECT_HPP


#include <glm/glm.hpp>
#include <string>
#include "../Ray.hpp"

namespace LumenRender {

    class Object {
    public:
        virtual ~Object() = default;

        enum Types {
            Sphere,
            Plane,
            Scene
        };

        virtual Types GetType() const = 0;
        virtual std::string GetName() const = 0;
        virtual HitRecords GetHitRecords() const = 0;
        virtual bool Hit(const Ray& ray, HitRecords& record) = 0;

        uint32_t m_Index{};
    };

    class Sphere : public Object {
    public:
        Sphere() = default;
        Sphere(const glm::vec3& center, float radius) : m_Center(center), m_Radius(radius) {}

        Types GetType() const override { return Types::Sphere; }

        std::string GetName() const override { return "Sphere"; }

        HitRecords GetHitRecords() const override { return *m_Record; }

        bool Hit(const Ray& ray, HitRecords& record) override;

    public:
        glm::vec3 m_Center{};
        const HitRecords* m_Record;
        float m_Radius{};
    };



    class Plane : public Object {
    public:
        Plane() = default;
        Plane(const glm::vec3& center, const glm::vec3& normal) : m_Center(center), m_Normal(normal) {}

        Types GetType() const override { return Types::Plane; }
        std::string GetName() const override { return "Plane"; }

        HitRecords GetHitRecords() const override { return HitRecords{m_Center, m_Index, m_Normal, 0.0f}; }

        bool Hit(const Ray& ray, HitRecords& record) override;

    public:
        glm::vec3 m_Center{};
        glm::vec3 m_Normal{};
        const HitRecords* m_Record;

    };
} // LumenRender

#endif //LUMEN_OBJECT_HPP