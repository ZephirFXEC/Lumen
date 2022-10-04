//
// Created by enzoc on 02/10/2022.
//

#ifndef LUMEN_SCENE_HPP
#define LUMEN_SCENE_HPP

#include "Object.hpp"

#include <memory>
#include <utility>
#include <vector>
#include <array>

namespace LumenRender {

    class Scene {
    public:
        Scene() = default;

        explicit Scene(std::vector<Object*>  objects) : m_Objects(std::move(objects)) {}

        void Add(Object *object) {
            m_Objects.push_back(object);
            m_Objects.back()->m_Index = m_Objects.size() - 1;
        }

        void Clear() {
            m_Objects.clear();
            free(m_Objects.data());
        }

        inline const std::vector<Object*>& GetObjects() const { return m_Objects; }

    private:
        std::vector<Object*> m_Objects;
    };

} // LumenRender

#endif //LUMEN_SCENE_HPP