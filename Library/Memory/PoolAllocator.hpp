#pragma once

#include <cassert>
#include <cstddef>

namespace usagi
{
/**
 * \brief An object pool which uses an internal container to store allocated
 * objects. The internal container holds a contiguous region of memory. When
 * the pool is exhausted, this allocator will try to expand the underlying
 * container. The container must ensure that the base address of the allocation
 * does not change, otherwise the behavior for a T that is not memcpy-able is
 * undefined after the reallocation.
 *
 * TODO: lock-free
 * todo: dump & load from file
 * todo: allow using memory-mapped file in place of the vector storage
 * \tparam T
 * \tparam Container
 */
template <
    typename T,
    template <typename> typename Container
>
class PoolAllocator
{
    constexpr static std::size_t INVALID_BLOCK = -1;

    union Block
    {
        std::size_t next = INVALID_BLOCK;
        std::byte data[sizeof T];
    };

    static_assert(sizeof Block == sizeof T);

    Container<Block> mStorage;
    std::size_t mFreeListHead = INVALID_BLOCK;

    Block & block(const std::size_t index)
    {
        return mStorage.at(index);
    }

    Block & head_free_block()
    {
        assert(!mStorage.empty());
        assert(mFreeListHead != INVALID_BLOCK);
        return block(mFreeListHead);
    }

public:
    PoolAllocator() = default;

    T & at(const std::size_t index)
    {
        auto &b = mStorage[index];
        return reinterpret_cast<T*>(&b.data);
    }

    template <typename... Args>
    std::size_t allocate(Args &&...args)
    {
        std::size_t idx = -1;
        // Free block available -> use it
        if(mFreeListHead != INVALID_BLOCK)
        {
            idx = mFreeListHead;
            mFreeListHead = head_free_block().next;
        }
        // No free block available -> push new block. may throw std::bad_alloc
        else
        {
            idx = mStorage.size();
            mStorage.emplace_back();
        }
        return idx;
    }

    void deallocate(const std::size_t index) noexcept
    {
        // Assume that the T in the block is correctly destructed,
        // which is none of our business.
        auto &fp = block(index);

        // Link the released block to the free list
        if(mFreeListHead == INVALID_BLOCK)
        {
            mFreeListHead = index;
        }
        else
        {
            fp.next = mFreeListHead;
            mFreeListHead = index;
        }
    }

    std::size_t size() const
    {
        return mStorage.size();
    }

    std::size_t capacity() const
    {
        return mStorage.capacity();
    }
};
}
