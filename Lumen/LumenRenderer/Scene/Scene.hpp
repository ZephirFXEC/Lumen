//
// Created by enzoc on 02/10/2022.
//

#ifndef LUMEN_SCENE_HPP
#define LUMEN_SCENE_HPP

#include "../Interfaces/IHittable.hpp"
#include "../Accelerators/Aabb.hpp"
#include "../Structure/Triangle.hpp"
#include "../Structure/Mesh.hpp"


#include <memory>
#include <unordered_map>
#include <mutex>

namespace LumenRender {

    class Scene : public IHittable {

    public:

        Scene() = default;

        [[nodiscard]] auto DeepCopy() const -> std::shared_ptr<IHittable> override;


        void AddObject(IHittable* object);
        void Clear() { m_Objects.clear(); }

        [[nodiscard]] auto GetObjects() const -> auto & { return m_Objects; }

        auto Hit(Ray &ray, float t_max) const -> bool override;
        auto GetBounds(AABB &outbox) const -> bool override;

    private:
        std::unordered_map<uint32_t, IHittable*> m_Objects;
        uint32_t m_Index = 0;
    };


} // LumenRender

#endif //LUMEN_SCENE_HPP