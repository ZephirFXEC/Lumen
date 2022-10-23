//
// Created by enzoc on 02/10/2022.
//

#ifndef LUMEN_SCENE_HPP
#define LUMEN_SCENE_HPP

#include "Object.hpp"
#include "../Accelerators/Aabb.hpp"
#include "../Structure/Triangle.hpp"
#include "../Structure/Mesh.hpp"


#include <memory>
#include <unordered_map>

namespace LumenRender {

    class Scene : public Object {
    public:
        Scene() = default;

        void AddObject(Object *object) {
            m_Objects.insert({ m_Index, object });
            m_Index++;
        }

        void AddObject(Mesh* mesh) {
            m_Objects.insert({ m_Index, mesh});
            m_Index++;
        }

        void Clear() {
            m_Objects.clear();
        }


        std::unordered_map<uint32_t, Object *> &GetObjects() {
            return m_Objects;
        }


        bool Hit(Ray &ray, float t_max) const override;

        bool GetBounds(AABB &outbox) const override;

    public:
        std::unordered_map<uint32_t, Object *> m_Objects;
        uint32_t m_Index = 0;
    };

} // LumenRender

#endif //LUMEN_SCENE_HPP