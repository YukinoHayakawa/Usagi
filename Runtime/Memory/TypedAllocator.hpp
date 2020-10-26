#pragma once

namespace usagi
{
template <typename T, typename AllocatorBase>
class TypedAllocator : public AllocatorBase
{
public:
    using AllocatorBase::AllocatorBase;
    using value_type = T;

    template <typename R>
    using rebind = TypedAllocator<R, AllocatorBase>;

    // can't assert on incomplete type
    // static_assert(ReallocatableAllocator<TypedAllocator>);
};
}
