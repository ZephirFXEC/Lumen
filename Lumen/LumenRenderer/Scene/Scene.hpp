//
// Created by enzoc on 02/10/2022.
//

#ifndef LUMEN_SCENE_HPP
#define LUMEN_SCENE_HPP

#include "Object.hpp"

#include <memory>
#include <unordered_map>

namespace LumenRender {

    class Scene : public Object {
    public:
        Scene() = default;
        explicit Scene(Object* object) { AddObject(object); }

        void AddObject(Object *object) {
            m_Objects.insert({m_Index, object});
            m_Index++;
        }

        void Clear() {
            m_Objects.clear();
        }

        //return a reference to the object
        Object *GetObject(uint32_t index) {
            return m_Objects[index];
        }

        virtual bool Hit(const Ray& ray, HitRecords& record) const override;

    public:
        std::unordered_map<uint32_t, Object*> m_Objects;
        uint32_t m_Index = 0;
    };

} // LumenRender

#endif //LUMEN_SCENE_HPP