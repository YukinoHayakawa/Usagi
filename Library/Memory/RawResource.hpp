#pragma once

#include <utility>

#include "Noncopyable.hpp"

namespace usagi
{
// Manages resource without proper RAII on its own.
template <
    typename InitFunc = void(*)(),
    typename DestroyFunc = void(*)()
>
class RawResource : Noncopyable
{
    InitFunc mInitFunc;
    DestroyFunc mDestroyFunc;
    bool mInitialized = false;

public:
    RawResource(InitFunc init_func, DestroyFunc destroy_func)
        : mInitFunc(std::move(init_func))
        , mDestroyFunc(std::move(destroy_func))
    {
        mInitFunc();
        mInitialized = true;
    }

    ~RawResource()
    {
        if(mInitialized)
            mDestroyFunc();
        mInitialized = false;
    }

    RawResource(RawResource &&other) noexcept
        : mInitFunc { std::move(other.mInitFunc) }
        , mDestroyFunc { std::move(other.mDestroyFunc) }
        , mInitialized { other.mInitialized }
    {
        other.mInitialized = false;
    }

    RawResource & operator=(RawResource &&other) noexcept
    {
        if(this == &other)
            return *this;
        mInitFunc = std::move(other.mInitFunc);
        mDestroyFunc = std::move(other.mDestroyFunc);
        mInitialized = other.mInitialized;
        other.mInitialized = false;
        return *this;
    }
};
}
