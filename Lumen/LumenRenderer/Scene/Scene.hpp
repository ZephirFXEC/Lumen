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
#include <mutex>

namespace LumenRender {

    class Scene : public Object {

    public:

        Scene() = default;
        Scene(Scene &other) = delete;

        static Scene *GetInstance();

        void AddObject(Object *object);
        void Clear() { m_Objects.clear(); }

        bool Hit(Ray &ray, float t_max) const override;
        bool GetBounds(AABB &outbox) const override;
        [[nodiscard]] ObjectType GetType() const override { return ObjectType::SCENE; }

    public:
        std::unordered_map<uint32_t, Object *> m_Objects;
        uint32_t m_Index = 0;

    private:
        static Scene *m_Instance;
        static std::mutex m_Mutex;
    };


} // LumenRender

#endif //LUMEN_SCENE_HPP