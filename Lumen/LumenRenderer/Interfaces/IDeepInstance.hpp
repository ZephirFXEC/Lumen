//
// Created by enzoc on 12/11/2022.
//

#ifndef LUMEN_IDEEPINSTANCE_HPP
#define LUMEN_IDEEPINSTANCE_HPP

#include <memory>

template<class T>
class IDeepInstance {
public:
    virtual ~IDeepInstance() = default;

    [[maybe_unused]] virtual auto DeepCopy() const -> std::shared_ptr<T> = 0;
};

#endif //LUMEN_IDEEPINSTANCE_HPP