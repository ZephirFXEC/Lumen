//
// Created by enzoc on 02/10/2022.
//

#ifndef LUMEN_SCENE_HPP
#define LUMEN_SCENE_HPP

#include "../Accelerators/Aabb.hpp"
#include "../Accelerators/Bvh.hpp"
#include "../Structure/Mesh.hpp"
#include "../Structure/Triangle.hpp"


#include <unordered_map>
#include <memory>
#include <mutex>
#include <variant>

namespace LumenRender {

using Types = std::variant<IHittable<Mesh> *, IHittable<Sphere> *, IHittable<BVH> *>;

class Scene : public IHittable<Scene>
{
  public:
    Scene() = default;

    auto AddObject(IHittable<Mesh> *object) -> void
    {
        m_Objects.insert({ m_Index, object });
        m_Index++;
    };

    auto Hit(LumenRender::Ray &ray, float t_max) const -> bool;

    auto CalculateBounds(LumenRender::AABB &outbox) const -> AABB;

    FORCEDINLINE auto GetObjects() -> std::unordered_map<uint32_t, IHittable<Mesh> *> & { return m_Objects; }

    [[nodiscard]] auto DeepCopy() const -> std::shared_ptr<IHittable>;


  private:
    std::unordered_map<uint32_t, IHittable<Mesh> *> m_Objects{};
    uint32_t m_Index = 0;
};


}// namespace LumenRender

#endif// LUMEN_SCENE_HPP