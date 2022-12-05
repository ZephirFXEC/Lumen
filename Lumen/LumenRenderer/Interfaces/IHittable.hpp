//
// Created by enzoc on 12/11/2022.
//

#ifndef LUMEN_IHITTABLE_HPP
#define LUMEN_IHITTABLE_HPP

#include "../Accelerators/Aabb.hpp"
#include "IDeepInstance.hpp"
#include <unordered_map>

template<class T> class IHittable : public IDeepInstance<IHittable<T>>
{
  public:
    IHittable() noexcept = default;
    ~IHittable() noexcept override = default;


    auto DeepCopy() const -> std::shared_ptr<IHittable>
    {
        return std::make_shared<IHittable>(static_cast<const IHittable &>(*this));
    }

    auto Hit(LumenRender::Ray &ray, float t_max) const -> bool { return static_cast<const T *>(this)->Hit(ray, t_max); }

    auto CalculateBounds(LumenRender::AABB &outbox) const -> LumenRender::AABB
    {
        return static_cast<const T *>(this)->CalculateBounds(outbox);
    }

    [[nodiscard]] auto GetBounds() const -> LumenRender::AABB { return static_cast<const T *>(this)->GetBounds(); }

};
#endif// LUMEN_IHITTABLE_HPP