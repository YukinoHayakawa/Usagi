#pragma once

#include <cassert>
#include <memory>
#include <utility>

#include <Usagi/Concept/Allocator/ReallocatableAllocator.hpp>
#include <Usagi/Concept/Type/Memcpyable.hpp>
#include <Usagi/Library/Math/Rounding.hpp>
#include <Usagi/Runtime/ErrorHandling.hpp>

namespace usagi
{
/**
 * \brief  A dynamic array that grows proportionally to system page size. It is
 * supposed to be used with memcpyable objects that are far smaller than
 * the page size. Metadata of the allocator is stored in the first page
 * of the allocation. Reallocation may change the base address.
 * \tparam T The element type.
 * \tparam Allocator
 */
template <
    Memcpyable T,
    ReallocatableAllocator Allocator
>
class DynamicArray
{
public:
    // types

    using value_type = T;
    using allocator_type = Allocator;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = std::size_t;

protected:
    Allocator mAllocator;

    // constexpr static std::uint64_t MAGIC_CHECK = 0x7899'4985'f7a0'4928;
    constexpr static std::uint16_t MAGIC_CHECK = 0x7899;
    constexpr static std::uint16_t NUM_CHECKS = 4;

    struct Meta
    {
        // this can be used by derived classes to insert more header checks
        std::uint16_t header_check[NUM_CHECKS] = { MAGIC_CHECK, 0, 0, 0 };
        std::uint64_t size = 0;
        std::uint64_t capacity = 0;
    };
    static_assert(sizeof(Meta) == 24);

    Meta *mBase = nullptr;
    T *mStorage = nullptr;

    // assert(mBase + page_size() == mStorage);

    constexpr static std::uint64_t HEADER_SIZE = 64; // 64 bytes
    constexpr static std::uint64_t ALLOCATION_SIZE = 0x10000; // 64 KiB
    constexpr static std::uint64_t EXTRA_HEADER_BEGIN = 0;

    void check_boundary_access(size_type n) const
    {
        assert(n < size());
    }

    bool storage_initialized() const
    {
        return mBase != nullptr;
    }

    template <const std::size_t Index, std::uint16_t Magic>
    void set_header_magic()
    {
        static_assert(Index < NUM_CHECKS);
        assert(storage_initialized());
        mBase->header_check[Index] = Magic;
    }

    template <const std::size_t Index, std::uint16_t Magic>
    void check_header_magic()
    {
        static_assert(Index < NUM_CHECKS);
        assert(storage_initialized());
        if(mBase->header_check[Index] != Magic)
        {
            USAGI_THROW(std::runtime_error("Corrupted header."));
        }
    }

    template <std::size_t Offset, typename Header>
    auto & extra_header_space()
    {
        assert(mBase);
        // the rest space in the header could be used by derived classes
        // to store associated info. beware don't write pass the header region.
        return *reinterpret_cast<Header*>(
            reinterpret_cast<char*>(mBase + 1) + Offset
        );
    }

public:
    // construct/copy/destroy

    DynamicArray() = default;

    DynamicArray(const DynamicArray &other) = delete;

    DynamicArray(DynamicArray &&other) noexcept
        : mAllocator { std::move(other.mAllocator) }
        , mBase { other.mBase }
        , mStorage { other.mStorage }
    {
        other.release();
    }

    DynamicArray & operator=(const DynamicArray &other) = delete;

    DynamicArray & operator=(DynamicArray &&other) noexcept
    {
        if(this == &other)
            return *this;
        mAllocator = std::move(other.mAllocator);
        mBase = other.mBase;
        mStorage = other.mStorage;
        other.release();
        return *this;
    }

    explicit DynamicArray(Allocator allocator) noexcept
        : mAllocator(std::move(allocator))
    {
    }

    // Move assignment & copy operations implicitly deleted.

    ~DynamicArray()
    {
        // unlink the metadata so it doesn't get cleared by clear()
        mBase = nullptr;
        if(mStorage) clear();
        mStorage = nullptr;
    }

    /*
    Allocator & allocator()
    {
        return mAllocator;
    }
    */

    // capacity
    [[nodiscard]] bool empty() const noexcept
    {
        return size() == 0;
    }

    size_type size() const noexcept
    {
        if(!mBase) return 0;
        return mBase->size;
    }

    size_type max_size() const noexcept
    {
        return (mAllocator.max_size() - HEADER_SIZE) / sizeof(T);
    }

    size_type capacity() const noexcept
    {
        if(!mBase) return 0;
        return mBase->capacity;
    }

    void shrink_to_fit()
    {
        reallocate_storage(size());
    }

    // element access
    reference operator[](size_type n)
    {
        check_boundary_access(n);
        return mStorage[n];
    }

    const_reference operator[](size_type n) const
    {
        check_boundary_access(n);
        return mStorage[n];
    }

    const_reference at(size_type n) const
    {
        check_boundary_access(n);
        return mStorage[n];
    }

    reference at(size_type n)
    {
        check_boundary_access(n);
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
        assert(size() > 0);
        return at(size() - 1);
    }

    const_reference back() const
    {
        assert(size() > 0);
        return at(size() - 1);
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
        --mBase->size;
    }

    void clear() noexcept
    {
        // if the storage is not initialized, size() would equal 0 and this
        // loop won't run anyway
        for(std::size_t i = 0; i < size(); ++i)
            std::allocator_traits<Allocator>::destroy(mAllocator, &mStorage[i]);
        if(mBase) mBase->size = 0;
    }

private:
    template <class... Args>
    reference emplace_back_reallocate(Args &&... args)
    {
        // strong exception guarantee (hopefully)

        const auto new_size = size() + 1;
        if(capacity() < new_size)
            reallocate_storage(new_size); // potentially throws

        T *storage = &mStorage[size()];
        std::allocator_traits<Allocator>::construct(
            mAllocator, storage, std::forward<Args>(args)...
        );

        // work is done. update bookkeeping data
        ++mBase->size;

        return *storage;
    }

    void reallocate_storage(const std::size_t size)
    {
        // strong exception guarantee

        const auto storage_size = sizeof(T) * size;
        const auto alloc_size = round_up_unsigned(
            HEADER_SIZE + storage_size, ALLOCATION_SIZE
        );

        // If the new capacity is smaller than the size, it is assumed that
        // the objects on the freed region are already correctly destructed.
        const auto new_storage = mAllocator.reallocate(mBase, alloc_size);
        rebase(new_storage, !storage_initialized());
        // the reminder part not making up an object is dropped
        mBase->capacity = storage_size / sizeof(T);
    }

    void release()
    {
        mStorage = nullptr;
        mBase = nullptr;
    }

protected:
    void rebase(void *base_address, const bool init_meta)
    {
        mBase = static_cast<Meta*>(base_address);
        mStorage = reinterpret_cast<T*>(
            static_cast<char*>(base_address) + HEADER_SIZE
        );

        if(init_meta) *mBase = Meta { };
        else check_header_magic<0, MAGIC_CHECK>();
    }
};
}
