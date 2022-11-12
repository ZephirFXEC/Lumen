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
    [[nodiscard]] std::shared_ptr<IHittable> DeepCopy() const override = 0;

    virtual bool Hit(LumenRender::Ray &ray, float t_max) const = 0;
    virtual bool GetBounds(LumenRender::AABB &outbox) const = 0;
};
#endif //LUMEN_IHITTABLE_HPP