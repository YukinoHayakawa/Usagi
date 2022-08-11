#pragma once

#include <cassert>
#include <cstddef>
#include <mutex>

namespace usagi
{
namespace details::pool
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
 * \tparam T Element type.
 * \tparam Container Underlying container.
 */
template <
    typename T,
    template <typename Elem, typename...> typename Container
>
class PoolAllocator : protected Container<details::pool::Block<T>>
{
    // A list that reuses the memory of the block to store link information.
    std::uint64_t mFreeListHead = INVALID_BLOCK;

protected:
    constexpr static std::size_t INVALID_BLOCK = details::pool::INVALID_BLOCK;

    using Block = details::pool::Block<T>;

public:
    using StorageT = Container<Block>;

protected:

    // todo lock free algorithm?
    // todo use lock state to detect data inconsistency caused by crash etc.
    std::mutex mMutex;

    // override if the value should be stored somewhere else
    virtual std::uint64_t & free_list_head()
    {
        return mFreeListHead;
    }

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
        , mFreeListHead { other.mFreeListHead }
        , mMutex { std::move(other.mMutex) }
    {
        other.release();
    }

    PoolAllocator & operator=(const PoolAllocator &other) = delete;

    PoolAllocator & operator=(PoolAllocator &&other) noexcept
    {
        if(this == &other)
            return *this;
        StorageT::operator=(std::move(other));
        mFreeListHead = std::move(other.mFreeListHead);
        mMutex = std::move(other.mMutex);
        other.release();
        return *this;
    }

    // todo: traverse and destroy elements
    virtual ~PoolAllocator() = default;

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

        std::unique_lock lock(mMutex);

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
        // which should be properly handled elsewhere.
        auto &fp = block(index);

        std::unique_lock lock(mMutex);

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
