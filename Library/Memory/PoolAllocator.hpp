#pragma once

#include <cassert>
#include <cstddef>

#include "SpinLock.hpp"
#include "LockGuard.hpp"

namespace usagi
{
namespace detail::pool_alloc
{
constexpr static std::size_t INVALID_BLOCK = -1;

template <typename T>
union Block
{
    std::size_t next = INVALID_BLOCK;
    T data;

    Block() { }
    ~Block() = default;

    // static_assert(sizeof Block == sizeof T);
};
}
/**
 * \brief An object pool which uses an internal container to store allocated
 * objects. The internal container holds a contiguous region of memory. When
 * the pool is exhausted, this allocator will try to expand the underlying
 * container. The container must ensure that the base address of the allocation
 * does not change, otherwise the behavior for a T that is not memcpy-able is
 * undefined after the reallocation.
 *
 * Object lifetime is NOT managed. The client must initialize the allocated
 * memory.
 *
 * TODO: lock-free
 * todo: dump & load from file
 * todo: allow using memory-mapped file in place of the vector storage
 * \tparam T
 * \tparam Container
 */
template <
    typename T,
    template <typename Elem> typename Container
>
class PoolAllocator
    : protected Container<detail::pool_alloc::Block<T>>
{

protected:
    constexpr static std::size_t INVALID_BLOCK =
        detail::pool_alloc::INVALID_BLOCK;

    using Block = detail::pool_alloc::Block<T>;

public:
    using StorageT = Container<Block>;

protected:
    struct Meta
    {
        std::uint64_t free_list_head = INVALID_BLOCK;
    } mMeta;

    // todo lock free algorithm?
    SpinLock mLock;

    auto & free_list_head() { return mMeta.free_list_head; };

    Block & block(const std::size_t index)
    {
        return StorageT::at(index);
    }

    Block & head_free_block()
    {
        assert(!StorageT::empty());
        assert(free_list_head() != INVALID_BLOCK);
        return block(free_list_head());
    }

    void release()
    {
        free_list_head() = INVALID_BLOCK;
    }

public:
    using value_type = T;

    PoolAllocator() = default;

    PoolAllocator(const PoolAllocator &other) = delete;

    PoolAllocator(PoolAllocator &&other) noexcept
        : StorageT { std::move(other) }
        , mMeta { other.mMeta }
        , mLock { std::move(other.mLock) }
    {
        other.release();
    }

    PoolAllocator & operator=(const PoolAllocator &other) = delete;

    PoolAllocator & operator=(PoolAllocator &&other) noexcept
    {
        if(this == &other)
            return *this;
        StorageT::operator=(std::move(other));
        free_list_head() = other.mFreeListHead;
        mLock = std::move(other.mLock);
        other.release();
        return *this;
    }

    template <typename R>
    using rebind = PoolAllocator<R, Container>;

    T & at(const std::size_t index)
    {
        auto &b = (*this)[index];
        return *reinterpret_cast<T*>(&b.data);
    }

    template <typename... Args>
    std::size_t allocate(Args &&...args)
    {
        std::size_t idx;

        LockGuard lock(mLock);

        // Free block available -> use it
        if(free_list_head() != INVALID_BLOCK)
        {
            idx = free_list_head();
            free_list_head() = head_free_block().next;
        }
        // No free block available -> push new block. may throw std::bad_alloc
        else
        {
            idx = StorageT::size();
            StorageT::emplace_back(std::forward<Args>(args)...);
        }

        return idx;
    }

    void deallocate(const std::size_t index) noexcept
    {
        // Assume that the T in the block is correctly destructed,
        // which is none of our business.
        auto &fp = block(index);

        LockGuard lock(mLock);

        // Link the released block to the free list
        fp.next = free_list_head();
        free_list_head() = index;
    }

    // return the number of nodes, not amount of active allocations
    std::size_t size() const
    {
        return StorageT::size();
    }

    std::size_t capacity() const
    {
        return StorageT::capacity();
    }
};
}
