#pragma once

#include <cassert>

namespace usagi
{
template <typename Lock>
class LockGuard
{
    Lock &mLock;
    bool mLocked = false;

public:
    explicit LockGuard(Lock &lock)
        : mLock(lock)
    {
        this->lock();
    }

    ~LockGuard()
    {
        if(mLocked)
            this->unlock();
    }

    void lock()
    {
        assert(!mLocked);

        mLock.lock();
        mLocked = true;
    }

    void unlock()
    {
        assert(mLocked);

        mLock.unlock();
        mLocked = false;
    }

    LockGuard(const LockGuard &other) = delete;
    LockGuard(LockGuard &&other) noexcept = delete;
    LockGuard & operator=(const LockGuard &other) = delete;
    LockGuard & operator=(LockGuard &&other) noexcept = delete;
};
}
