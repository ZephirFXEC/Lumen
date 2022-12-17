// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#ifndef LUMEN_IDEEPINSTANCE_HPP
#define LUMEN_IDEEPINSTANCE_HPP

#include <memory>

template<class T> class IDeepInstance
{
  public:
    IDeepInstance() = default;

    virtual ~IDeepInstance() = default;

    [[maybe_unused]] auto DeepCopy() const -> std::shared_ptr<T>
    {
        return std::make_shared<T>(static_cast<const T &>(*this));
    }
};

#endif// LUMEN_IDEEPINSTANCE_HPP