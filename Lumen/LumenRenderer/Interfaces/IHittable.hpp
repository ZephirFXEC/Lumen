// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#ifndef LUMEN_IHITTABLE_HPP
#define LUMEN_IHITTABLE_HPP

#include "../Accelerators/Aabb.hpp"
#include "IDeepInstance.hpp"
#include <optional>

template<class T> class IHittable : public IDeepInstance<IHittable<T>>
{
  public:
    IHittable() noexcept = default;
    ~IHittable() noexcept override = default;


    auto DeepCopy() const -> std::shared_ptr<IHittable>
    {
        return std::make_shared<IHittable>(static_cast<const IHittable &>(*this));
    }

    constexpr auto Hit(const LumenRender::Ray &ray, float t_max) const -> bool
    {
        return static_cast<const T *>(this)->Hit(ray, t_max);
    }
    constexpr auto CalculateBounds(LumenRender::AABB &outbox) const -> LumenRender::AABB
    {
        return static_cast<const T *>(this)->CalculateBounds(outbox);
    }

    [[nodiscard]] constexpr auto GetBounds() const -> LumenRender::AABB
    {
        return static_cast<const T *>(this)->GetBounds();
    }
};
#endif// LUMEN_IHITTABLE_HPP