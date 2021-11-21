#pragma once

#include <cassert>
#include <memory>
#include <mutex>

namespace usagi
{
template <typename T>
class WeakSingleton
{
    inline static std::recursive_mutex mMutex;
    inline static std::weak_ptr<T> mInstance;

public:
    static auto lock()
    {
        std::lock_guard lk(mMutex);
        return mInstance.lock();
    }

    static bool alive()
    {
        std::lock_guard lk(mMutex);
        return !mInstance.expired();
    }

    template <typename... Args>
    [[nodiscard]]
    static auto construct(Args &&... args)
    {
        assert(!alive());

        std::lock_guard lk(mMutex);

        auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
        mInstance = ptr;
        return ptr;
    }

    template <typename... Args>
    [[nodiscard]]
    static auto try_lock_construct(Args &&... args)
    {
        std::lock_guard lk(mMutex);
        if(auto ptr = lock()) return ptr;
        return construct(std::forward<Args>(args)...);
    }
};
}
