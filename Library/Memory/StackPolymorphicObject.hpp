#pragma once

#include <cstddef>
#include <type_traits>

#include <Usagi/Runtime/ErrorHandling.hpp>

#include "Noncopyable.hpp"
#include "Nonmovable.hpp"

namespace usagi
{
template <
    typename BaseT,
    std::size_t MaxSize = 64
>
class StackPolymorphicObject : Noncopyable, Nonmovable
{
    // To move an object built on stack, the object should be provided
    // to the move operator, but we only know its base type so slicing may
    // happen. Release the pointer to object memory makes no sense as it's
    // on stack. So this class is declared as nonmovable.

    alignas(BaseT) char mBuffer[MaxSize] { };
    bool mInitialized = false;

public:
    StackPolymorphicObject() = default;

    // This ctor makes no sense as if we know the derived type upon construction
    // we won't need this class in the first place.
    // template <typename DerivedT, typename... Args>
    // StackPolymorphicObject(Args &&...args)
    // {
    //     construct<DerivedT>(std::forward<Args>(args)...);
    // }

    template <typename DerivedT, typename... Args>
    void construct(Args &&...args)
        requires std::is_base_of_v<BaseT, DerivedT>
        && (sizeof(DerivedT) <= MaxSize)
    {
        if(mInitialized != false)
            USAGI_THROW(std::bad_alloc());

        new (mBuffer) DerivedT(std::forward<Args>(args)...);
        mInitialized = true;
    }

    operator bool() const
    {
        return mInitialized;
    }

    bool has_value() const
    {
        return mInitialized;
    }

    ~StackPolymorphicObject()
    {
        // Ensure correct dtor behavior.
        static_assert(std::has_virtual_destructor_v<BaseT>);

        if(mInitialized)
            get()->~BaseT();
        mInitialized = false;
    }

    BaseT * get()
    {
        if(mInitialized)
            return reinterpret_cast<BaseT *>(mBuffer);
        return nullptr;
    }

    const BaseT * get() const
    {
        if(mInitialized)
            return reinterpret_cast<const BaseT *>(mBuffer);
        return nullptr;
    }

    BaseT * operator->()
    {
        return get();
    }

    const BaseT * operator->() const
    {
        return get();
    }
};
}
