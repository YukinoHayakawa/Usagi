#pragma once

#include <typeinfo>

#include <Usagi/Runtime/ErrorHandling.hpp>

namespace usagi
{
template <typename DerivedT, typename PtrT>
DerivedT& dynamic_cast_ref_throw(const PtrT& ptr)
{
    auto target = dynamic_cast<DerivedT*>(ptr);
    if (target == nullptr)
        USAGI_THROW(std::bad_cast());
    return *target;
}
}
