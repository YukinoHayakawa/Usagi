#pragma once

#include <cassert>
#include <memory>
#include <utility>

#include <Usagi/Concepts/Allocator/ReallocatableAllocator.hpp>
#include <Usagi/Concepts/Type/Memcpyable.hpp>
#include <Usagi/Library/Memory/Alignment.hpp>
#include <Usagi/Runtime/ErrorHandling.hpp>
#include <Usagi/Runtime/Exceptions/ExceptionHeaderCorruption.hpp>

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

    // dynamic array
    constexpr static std::uint16_t MAGIC_CHECK = 0xDA01;
    constexpr static std::uint16_t MAX_HEADER_DEPTH = 4;

    // the rest space in the header could be used by derived classes
    // to store associated info.
    struct Meta
    {
        // this can be used by derived classes to insert more header checks
        std::uint16_t header_check[MAX_HEADER_DEPTH] = { };
        std::uint64_t size = 0;
        std::uint64_t capacity = 0;
    };
    static_assert(sizeof(Meta) == 24);

    Meta *mBase = nullptr;
    T *mStorage = nullptr;

    // assert(mBase + page_size() == mStorage);

    constexpr static std::uint64_t MAX_HEADER_SIZE = 64; // 64 bytes
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

    std::uint8_t mHeaderIndex = 0;
    std::uint8_t mHeaderOffset = 0;

    // push header stack and restore existing data from mapped memory
    template <std::uint16_t Magic, typename Header>
    void push_header(Header &header, const bool restore)
    {
        assert(mHeaderIndex < MAX_HEADER_DEPTH);
        assert(mHeaderOffset + sizeof(Header) <= MAX_HEADER_SIZE);
        if(restore)
        {
            assert(storage_initialized());
            if(mBase->header_check[mHeaderIndex] != Magic)
                USAGI_THROW(ExceptionHeaderCorruption("Corrupted header."));
            header = *reinterpret_cast<Header*>(
                reinterpret_cast<char*>(mBase) + mHeaderOffset
            );
        }
        ++mHeaderIndex;
        mHeaderOffset += sizeof(Header);
    }

    // pop header stack and store data to mapped memory
    template <std::uint16_t Magic, typename Header>
    void pop_header(Header &header, const bool save)
    {
        assert(mHeaderIndex > 0);
        assert(mHeaderOffset >= sizeof(Header));
        --mHeaderIndex;
        mHeaderOffset -= sizeof(Header);
        if(save && storage_initialized())
        {
            mBase->header_check[mHeaderIndex] = Magic;
            *reinterpret_cast<Header*>(
                reinterpret_cast<char*>(mBase) + mHeaderOffset
            ) = header;
        }
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
        if(mBase) pop_header<MAGIC_CHECK>(*mBase, false);
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
        return (mAllocator.max_size() - MAX_HEADER_SIZE) / sizeof(T);
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
        const auto alloc_size = align_up(
            MAX_HEADER_SIZE + storage_size, ALLOCATION_SIZE
        );

        // If the new capacity is smaller than the size, it is assumed that
        // the objects on the freed region are already correctly destructed.
        const auto new_storage = mAllocator.reallocate(mBase, alloc_size);
        rebase(new_storage, !storage_initialized());
        // the reminder part not making up an object is dropped
        mBase->capacity = (alloc_size - MAX_HEADER_SIZE) / sizeof(T);
    }

    void release()
    {
        mStorage = nullptr;
        mBase = nullptr;
    }

protected:
    void rebase(void *base_address, const bool init_meta)
    {
        const bool first_alloc = !storage_initialized();

        mBase = static_cast<Meta*>(base_address);
        mStorage = reinterpret_cast<T*>(
            static_cast<char*>(base_address) + MAX_HEADER_SIZE
        );

        if(init_meta) *mBase = Meta { };
        // if this is first allocation, initialize the header
        if(first_alloc) push_header<MAGIC_CHECK>(*mBase, false);
    }
};
}
