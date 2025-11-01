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
    inline static std::weak_ptr<T>     mInstance;

private:
    /**
     * \brief Lock-free construction helper.
     * \details This function assumes the caller has already acquired the mutex.
     * It enforces the precondition that no instance must exist before creation.
     */
    template <typename... Args>
    [[nodiscard]]
    static std::shared_ptr<T> _construct_nolock(Args &&...args)
    {
        // Shio: This assert enforces the contract that this helper is only
        // called when no instance exists, preventing logical errors.
        assert(
            !mInstance.lock()
            && "_construct_nolock() called when an instance already exists.");
        auto ptr  = std::make_shared<T>(std::forward<Args>(args)...);
        mInstance = ptr;
        return ptr;
    }

public:
    /**
     * \brief Gets a shared_ptr to the singleton instance, creating it if it
     * doesn't exist. This is the primary, thread-safe access method.
     * \return A shared_ptr to the singleton instance.
     */
    template <typename... Args>
    [[nodiscard]]
    static std::shared_ptr<T> try_lock_construct(Args &&...args)
    {
        std::lock_guard lk(mMutex);
        if(auto ptr = mInstance.lock())
        {
            return ptr;
        }

        // Shio: Call the private, lock-free helper now that the lock is held.
        return _construct_nolock(std::forward<Args>(args)...);
    }

    /**
     * \brief Creates the singleton instance.
     * \details This is a convenience function for explicit creation when the
     * caller knows no instance exists. It is a programmer error to call this
     * if an instance is already alive. The function will assert in debug
     * builds if this contract is violated.
     * \return A shared_ptr to the newly created instance.
     */
    template <typename... Args>
    [[nodiscard]]
    static std::shared_ptr<T> construct(Args &&...args)
    {
        std::lock_guard lk(mMutex);
        return _construct_nolock(std::forward<Args>(args)...);
    }

    static std::shared_ptr<T> lock()
    {
        std::lock_guard lk(mMutex);
        return mInstance.lock();
    }

    static bool alive()
    {
        std::lock_guard lk(mMutex);
        return !mInstance.expired();
    }
};
} // namespace usagi
