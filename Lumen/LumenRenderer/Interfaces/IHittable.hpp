//
// Created by enzoc on 12/11/2022.
//

#ifndef LUMEN_IHITTABLE_HPP
#define LUMEN_IHITTABLE_HPP

#include "IDeepInstance.hpp"
#include "../Accelerators/Aabb.hpp"

class IHittable : public IDeepInstance<IHittable> {
public:
    ~IHittable() noexcept override = default;
    [[nodiscard]] auto DeepCopy() const -> std::shared_ptr<IHittable> override = 0;

    virtual auto Hit(LumenRender::Ray &ray, float t_max) const -> bool = 0;
    virtual auto GetBounds(LumenRender::AABB &outbox) const -> bool = 0;
};
#endif //LUMEN_IHITTABLE_HPP