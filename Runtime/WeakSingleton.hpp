#pragma once

#include <cassert>
#include <memory>

namespace usagi
{
template <typename T>
class WeakSingleton
{
    inline static std::weak_ptr<T> mInstance;

public:
    static auto lock()
    {
        return mInstance.lock();
    }

    static bool alive()
    {
        return !mInstance.expired();
    }

    template <typename... Args>
    [[nodiscard]]
    static auto construct(Args &&... args)
    {
        assert(!alive());

        auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
        mInstance = ptr;
        return ptr;
    }

    template <typename... Args>
    [[nodiscard]]
    static auto try_lock_construct(Args &&... args)
    {
        if(auto ptr = lock()) return ptr;
        return construct(std::forward<Args>(args)...);
    }
};
}
