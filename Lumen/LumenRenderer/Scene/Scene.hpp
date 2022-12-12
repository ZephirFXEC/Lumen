//
// Created by enzoc on 02/10/2022.
//

#ifndef LUMEN_SCENE_HPP
#define LUMEN_SCENE_HPP

#include "../Accelerators/Aabb.hpp"
#include "../Accelerators/Bvh.hpp"
#include "../Ray.hpp"
#include "../Structure/Mesh.hpp"
#include "../Structure/Triangle.hpp"

#include <unordered_map>

namespace LumenRender {

class Scene : public IHittable<Scene>
{
  public:
    Scene() = default;

    auto AddObject(IHittable<Mesh> *object) -> void
    {
        m_Objects.insert({ m_Index, object });
        m_Index++;
    };

    auto Hit(Ray &ray, float t_max) const -> bool;

    auto CalculateBounds(AABB &outbox) const -> AABB;

    __forceinline auto GetObjects() -> std::unordered_map<uint32_t, IHittable<Mesh> *> const & { return m_Objects; }

    [[nodiscard]] auto DeepCopy() const -> std::shared_ptr<IHittable>;


  private:
    std::unordered_map<uint32_t, IHittable<Mesh> *> m_Objects{};
    uint32_t m_Index = 0;
};


}// namespace LumenRender

#endif// LUMEN_SCENE_HPP