#pragma once

#include <cassert>
#include <utility>
#include <memory>

#include <Usagi/Library/Concept/ReallocatableAllocator.hpp>
#include <Usagi/Runtime/Memory/VirtualMemoryAllocator.hpp>

namespace usagi
{
/**
 * \brief A simple dynamic array which relies on the allocator's ability to
 * reallocate memory to grow the storage in-place. If the allocator cannot
 * handle the reallocation, further element insertions fail. The capacity grows
 * linearly with the amount of stored elements, means that it only grows the
 * size of storage by one element each time. The time complexity of the
 * insertion functions is thus dependent on the efficiency of the reallocate
 * function of the allocator.
 * If the allocator changes the base address during reallocation, the behavior
 * is undefined for non-memcpy-able contained types.
 * \tparam T The element type.
 * \tparam Allocator
 */
template <
    typename T,
    ReallocatableAllocator Allocator = VirtualMemoryAllocator<T>
>
class DynamicArray
{
    Allocator mAllocator;

    T *mStorage = nullptr;
    std::size_t mSize = 0;
    std::size_t mCapacity = 0;

public:
    // types

    using value_type = T;
    using allocator_type = Allocator;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = std::size_t;

    template <typename R>
    using rebind = DynamicArray<R, Allocator>;

    // construct/copy/destroy

    DynamicArray() = default;

    explicit DynamicArray(Allocator allocator) noexcept
        : mAllocator(std::move(allocator))
    {
    }

    DynamicArray(DynamicArray &&other) noexcept
        : mAllocator(std::move(other.mAllocator))
        , mStorage(other.mStorage)
        , mSize(other.mSize)
        , mCapacity(other.mCapacity)
    {
        other.release();
    }

    // Move assignment & copy operations implicitly deleted.

    ~DynamicArray()
    {
        if(mStorage)
            clear();
    }

    // capacity
    [[nodiscard]] bool empty() const noexcept
    {
        return mSize == 0;
    }

    size_type size() const noexcept
    {
        return mSize;
    }

    size_type max_size() const noexcept
    {
        return mAllocator.max_size() / sizeof(T);
    }

    size_type capacity() const noexcept
    {
        return mCapacity;
    }

    void shrink_to_fit()
    {
        reallocate_storage(mSize);
    }

    // Let the allocator reserve the memory range without committing.
    // This shall only be called once and only if the allocator is not
    // initialized.
    void allocator_reserve(const std::size_t bytes)
    {
        mAllocator.reserve(bytes);
    }

    // element access
    reference operator[](size_type n)
    {
        assert(n < mSize);
        return mStorage[n];
    }

    const_reference operator[](size_type n) const
    {
        assert(n < mSize);
        return mStorage[n];
    }

    const_reference at(size_type n) const
    {
        assert(n < mSize);
        return mStorage[n];
    }

    reference at(size_type n)
    {
        assert(n < mSize);
        return mStorage[n];
    }

    reference front()
    {
        return at(0);
    }

    const_reference front() const
    {
        return at(0);
    }

    reference back()
    {
        assert(mSize > 0);
        return at(mSize - 1);
    }

    const_reference back() const
    {
        assert(mSize > 0);
        return at(mSize - 1);
    }

    // data access
    T * data() noexcept
    {
        return mStorage;
    }

    const T * data() const noexcept
    {
        return mStorage;
    }

    // modifiers
    template <class... Args>
    reference emplace_back(Args &&... args)
    {
        T &ret = emplace_back_reallocate(std::forward<Args>(args)...);
        return ret;
    }

    void push_back(const T &x)
    {
        emplace_back(x);
    }

    void push_back(T &&x)
    {
        emplace_back(std::move(x));
    }

    void pop_back()
    {
        std::allocator_traits<Allocator>::destroy(mAllocator, &back());
        --mSize;
    }

    void clear() noexcept
    {
        for(std::size_t i = 0; i < mSize; ++i)
            std::allocator_traits<Allocator>::destroy(mAllocator, &mStorage[i]);
        mSize = 0;
    }

private:
    template <class... Args>
    reference emplace_back_reallocate(Args &&... args)
    {
        // strong exception guarantee

        const auto new_size = mSize + 1;
        if(mCapacity < new_size)
            reallocate_storage(new_size); // potentially throws

        T *storage = &mStorage[mSize];
        std::allocator_traits<Allocator>::construct(
            mAllocator, storage, std::forward<Args>(args)...
        );

        // work is done. update bookkeeping data

        ++mSize;

        return *storage;
    }

    void reallocate_storage(const std::size_t size)
    {
        // strong exception guarantee

        // If the new capacity is smaller than the size, it is assumed that
        // the objects on the freed region are already correctly destructed.
        const auto new_storage =
            mAllocator.reallocate(mStorage, sizeof(T) * size);
        if(mStorage == nullptr)
            mStorage = static_cast<T*>(new_storage);
        assert(new_storage == static_cast<void*>(mStorage));

        mCapacity = size;
    }

    void release()
    {
        mStorage = nullptr;
        mSize = 0;
        mCapacity = 0;
    }
};
}
