// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#ifndef LUMEN_IHITTABLE_HPP
#define LUMEN_IHITTABLE_HPP

#include "../Accelerators/Aabb.hpp"
#include <optional>

template<class T> class IHittable
{
  public:
    IHittable() noexcept = default;
    ~IHittable() noexcept = default;

    [[nodiscard]] constexpr auto Hit(const LumenRender::Ray &ray, float t_max) const -> bool
    {
        return static_cast<const T *>(this)->Hit(ray, t_max);
    }

    [[nodiscard]] constexpr auto GetBounds() const -> LumenRender::AABB
    {
        return static_cast<const T *>(this)->GetBounds();
    }
};
#endif// LUMEN_IHITTABLE_HPP