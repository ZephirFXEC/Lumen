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



} // LumenRender

#endif //LUMEN_OBJECT_HPP