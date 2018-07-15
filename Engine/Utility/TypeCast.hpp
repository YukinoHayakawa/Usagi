#pragma once

#include <memory>
#include <type_traits>

namespace usagi
{
template <typename Target, typename Source>
std::shared_ptr<Target> dynamic_pointer_cast_throw(
    const std::shared_ptr<Source> &source)
{
    auto result = std::dynamic_pointer_cast<Target>(source);
    if(!result)
        throw std::bad_cast();
    return result;
}

template <typename Instance, typename T>
bool is_instance_of(T *ptr)
{
    using RealT = std::decay_t<T>;
    static_assert(sizeof(RealT) != 0, "T is not defined in the context.");
    static_assert(std::is_polymorphic_v<RealT>, "T is not polymorphic.");
    return dynamic_cast<const Instance*>(ptr) != nullptr;
}
}
