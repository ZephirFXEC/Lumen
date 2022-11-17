//
// Created by enzoc on 02/10/2022.
//

#ifndef LUMEN_SCENE_HPP
#define LUMEN_SCENE_HPP

#include "../Accelerators/Aabb.hpp"
#include "../Structure/Triangle.hpp"
#include "../Structure/Mesh.hpp"


#include <memory>
#include <unordered_map>
#include <mutex>
#include <variant>

namespace LumenRender {

    using Types = std::variant<Mesh*, Sphere*>;

    class Scene : public IHittable<Scene> {
    public:

        Scene() = default;

        auto DeepCopy() const -> std::shared_ptr<IHittable>;

        auto AddObject(Types object) -> void {
            m_Objects.insert({ m_Index, object});
            m_Index++;
        };

        auto Hit(LumenRender::Ray &ray, float t_max) const -> bool;

        auto GetBounds(LumenRender::AABB &outbox) const -> bool;


    private:

        std::unordered_map<uint32_t, Types> m_Objects{};
        uint32_t m_Index = 0;

    };


} // LumenRender

#endif //LUMEN_SCENE_HPP