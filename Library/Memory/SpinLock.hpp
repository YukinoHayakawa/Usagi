#pragma once

#include <atomic>

namespace usagi
{
class SpinLock
{
    // https://en.cppreference.com/w/cpp/atomic/atomic_flag
    std::atomic_flag mLock = ATOMIC_FLAG_INIT;

public:
    void lock()
    {
        while(mLock.test_and_set(std::memory_order_acquire)) // acquire lock
            ; // spin
    }

    void unlock()
    {
        mLock.clear(std::memory_order_release); // release lock
    }
};
}
