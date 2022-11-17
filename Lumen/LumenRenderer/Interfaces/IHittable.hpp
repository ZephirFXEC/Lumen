//
// Created by enzoc on 12/11/2022.
//

#ifndef LUMEN_IHITTABLE_HPP
#define LUMEN_IHITTABLE_HPP

#include <unordered_map>
#include "IDeepInstance.hpp"
#include "../Accelerators/Aabb.hpp"

template<class T>
class IHittable : public IDeepInstance<IHittable<T>> {
public:
    ~IHittable() noexcept override = default;

    auto DeepCopy() const -> std::shared_ptr<IHittable> {
        return std::make_shared<IHittable>(static_cast<const IHittable &>(*this));
    }

    auto Hit(LumenRender::Ray &ray, float t_max) const -> bool {
        return static_cast<const T *>(this)->Hit(ray, t_max);
    }

    auto GetBounds(LumenRender::AABB &outbox) const -> bool {
        return static_cast<const T *>(this)->GetBounds(outbox);
    }
};
#endif //LUMEN_IHITTABLE_HPP