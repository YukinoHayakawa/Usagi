#pragma once

#include <memory>
#include <type_traits>

#include <Usagi/Core/Exception.hpp>

namespace usagi
{
template <typename Target, typename Source>
std::shared_ptr<Target> dynamic_pointer_cast_throw(
    const std::shared_ptr<Source> &source)
{
    auto result = std::dynamic_pointer_cast<Target>(source);
    if(!result)
        USAGI_THROW(std::bad_cast());
    return std::move(result);
}

template <typename Instance, typename T>
bool is_instance_of(T *ptr)
{
    using RealT = std::decay_t<T>;
    static_assert(sizeof(RealT) != 0, "T is not defined in the context.");
    static_assert(std::is_polymorphic_v<RealT>, "T is not polymorphic.");
    return dynamic_cast<const Instance*>(ptr) != nullptr;
}

template <typename Target, typename Source>
std::unique_ptr<Target> unique_pointer_cast(std::unique_ptr<Source> ptr)
{
    return std::unique_ptr<Target> { static_cast<Target*>(ptr.release()) };
}

template <typename DerivedT, typename PtrT>
DerivedT & dynamic_cast_ref(const PtrT &ptr)
{
    return dynamic_cast<DerivedT&>(*ptr);
}
}
