#pragma once

#include <deque>
#include <mutex>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::memory
{
/**
 * \brief A thread-safe FIFO allocator for managing remote memory,
 * also supports out-of-order deallocation.
 * 
 * Structure:
 * [ tail | allocated | head ]
 */
class CircularAllocator : Noncopyable
{
    struct Allocation
    {
        // starting offset of data area
        uint32_t offset = 0;
        // length of data area
        uint32_t length = 0;
        // size of padding for alignment before the data
        uint16_t alignment_padding = 0;

        enum class State : uint16_t
        {
            FREE,
            USED,
            PENDING_FREE
        } state = State::FREE;
    };

    std::deque<Allocation> mAllocations;
    char *const mBegin = nullptr;
    const std::size_t mSize = 0;
    char *mTail = nullptr; // next deallocation position
    char *mHead = nullptr; // next allocation position
    std::mutex mAllocMutex;

    bool tryAllocateFromRange(std::size_t num_bytes,
        std::size_t alignment,
        char *begin, char *end, Allocation &alloc) const;
    char *wrapIncrement(char *original, std::size_t increment) const;

public:
    CircularAllocator(void *begin, std::size_t size);

    void * allocate(std::size_t num_bytes, std::size_t alignment = 0);
    void deallocate(void *pointer);
};
}
