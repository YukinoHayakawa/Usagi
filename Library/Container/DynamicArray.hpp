#pragma once

#include <cassert>
#include <memory>
#include <utility>

#include <Usagi/Concepts/Allocator/ReallocatableAllocator.hpp>
#include <Usagi/Concepts/Type/Memcpyable.hpp>
#include <Usagi/Library/Memory/Alignment.hpp>
#include <Usagi/Library/Memory/Arithmetic.hpp>
#include <Usagi/Runtime/ErrorHandling.hpp>
#include <Usagi/Runtime/Exceptions/ExceptionHeaderCorruption.hpp>

namespace usagi
{
/**
 * \brief A dynamic array that allocates by a multiple of system page size.
 * It is supposed to be used with memcpy-able objects that are far smaller than
 * the page size. Metadata of the allocator is stored in the first page
 * of the allocation. Reallocation may result in base address changes. 
 * A stack of headers is maintained to record the structure of internally
 * stored data. Derived classes can use it to store additional data.
 * \tparam T The element type.
 * \tparam Allocator Allocator type.
 */
template <
    Memcpyable T,
    ReallocatableAllocator Allocator
>
class DynamicArray
{
public:
    // traits

    using value_type = T;
    using allocator_type = Allocator;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = std::size_t;

    using AllocatorT = Allocator;

protected:
    Allocator mAllocator;

    using HeaderMagicT = std::uint64_t;

    // rabbit (usagi) engine .... dynamic array version 1
    constexpr static HeaderMagicT MAGIC_CHECK = 0xABBE'F5A9B8FF'DA01;
    constexpr static std::uint64_t MAX_HEADER_SIZE = 256; // 256 bytes
    constexpr static std::uint64_t ALLOCATION_SIZE = 0x10000; // 64 KiB

    // the rest space in the header could be used by derived classes
    // to store associated info.
    struct Header
    {
        std::uint64_t storage_offset = 0;
        std::uint64_t capacity = 0;
        std::uint64_t size = 0;
    };
    static_assert(sizeof(Header) < MAX_HEADER_SIZE);

    // Metadata and storage are stored in same chunk of allocation.
    void *mStorage = nullptr;
    // Cached for faster access. value may change after reallocation.
    T *mElements = nullptr;

    // assert(mBase + page_size() == mStorage);

    template <typename HeaderT = Header>
    HeaderT * header(std::size_t offset = sizeof(HeaderMagicT)) const
    {
        assert(storage_initialized());
        return raw_advance_cast<HeaderT>(mStorage, offset);
    }

    void check_boundary_access(size_type n) const
    {
        assert(n < size());
    }

    bool storage_initialized() const
    {
        return mStorage != nullptr;
    }

    // std::uint8_t mHeaderDepth = 0;
    std::uint8_t mCommittedHeaderOffset = 0;

    /**
     * \brief Initialize or restore header from allocated storage.
     * \tparam Magic Magic number.
     * \tparam Header Header type.
     * \return <offset to header, initialized>
     */
    template <HeaderMagicT Magic, typename Header>
    std::pair<std::size_t, bool> init_or_restore_header()
    {
        constexpr auto header_size_with_magic =
            sizeof(HeaderMagicT) + sizeof(Header);

        // make sure not overwriting element payload
        assert(mCommittedHeaderOffset + header_size_with_magic <= MAX_HEADER_SIZE);

        HeaderMagicT *magic_probe = raw_advance_cast<HeaderMagicT>(
            mStorage,
            mCommittedHeaderOffset
        );
        const auto header_payload_offset =
            mCommittedHeaderOffset + sizeof(HeaderMagicT);
        Header *header_probe = raw_advance_cast<Header>(
            mStorage,
            header_payload_offset
        );

        bool initialized;
        // header magic matches, return the header
        if(*magic_probe == Magic)
        {
            initialized = false;
        }
        // zeroed storage, init header
        else if(*magic_probe == 0)
        {
            *magic_probe = Magic;
            *header_probe = Header { };
            initialized = true;
        }
        // magic unmatch, throw.
        else
        {
            USAGI_THROW(ExceptionHeaderCorruption("Corrupted header."));
        }
        mCommittedHeaderOffset += header_size_with_magic;
        return { header_payload_offset, initialized };
    }

public:
    // construct/copy/destroy

    DynamicArray() = default;

    DynamicArray(const DynamicArray &other) = delete;

    DynamicArray(DynamicArray &&other) noexcept
        : mAllocator { std::move(other.mAllocator) }
        , mStorage { other.mStorage }
        , mElements { other.mElements }
    {
        other.release();
    }

    DynamicArray & operator=(const DynamicArray &other) = delete;

    DynamicArray & operator=(DynamicArray &&other) noexcept
    {
        if(this == &other)
            return *this;
        mAllocator = std::move(other.mAllocator);
        mStorage = other.mStorage;
        mElements = other.mElements;
        other.release();
        return *this;
    }

    explicit DynamicArray(Allocator allocator)
    {
        init(std::move(allocator));
    }

    void init(Allocator allocator)
    {
        assert(!storage_initialized());
        mAllocator = std::move(allocator);
        // allocate header
        const auto alloc_size = calc_allocation_size(0);
        mStorage = mAllocator.reallocate(nullptr, calc_allocation_size(0));
        // if this is a newly created container, initialize the header
        if(auto [header_offset, newly_initialized] =
            init_or_restore_header<MAGIC_CHECK, Header>(); newly_initialized)
        {
            header()->storage_offset = MAX_HEADER_SIZE;
            update_capacity(alloc_size);
        }
        update_elem_array_ptr();
    }

    // Move assignment & copy operations implicitly deleted.

    ~DynamicArray()
    {
        release();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return size() == 0;
    }

    size_type size() const noexcept
    {
        return header()->size;
    }

    size_type max_size() const noexcept
    {
        return (mAllocator.max_size() - MAX_HEADER_SIZE) / sizeof(T);
    }

    size_type capacity() const noexcept
    {
        return header()->capacity;
    }

    void shrink_to_fit()
    {
        reallocate_storage(size());
    }

    // element access
    reference operator[](size_type n)
    {
        check_boundary_access(n);
        return mElements[n];
    }

    const_reference operator[](size_type n) const
    {
        check_boundary_access(n);
        return mElements[n];
    }

    const_reference at(size_type n) const
    {
        check_boundary_access(n);
        return mElements[n];
    }

    reference at(size_type n)
    {
        check_boundary_access(n);
        return mElements[n];
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
        return mElements;
    }

    const T * data() const noexcept
    {
        return mElements;
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
        --header()->size;
    }

    void clear() noexcept
    {
        // if the storage is not initialized, size() would equal 0 and this
        // loop won't run anyway
        for(std::size_t i = 0; i < size(); ++i)
            std::allocator_traits<Allocator>::destroy(mAllocator, &mElements[i]);
        header()->size = 0;
    }

private:
    template <class... Args>
    reference emplace_back_reallocate(Args &&... args)
    {
        const auto new_size = size() + 1;
        if(capacity() < new_size)
            reallocate_storage(new_size); // potentially throws

        T *storage = &mElements[size()];
        std::allocator_traits<Allocator>::construct(
            mAllocator, storage, std::forward<Args>(args)...
        );

        // work is done. update bookkeeping data
        ++header()->size;

        return *storage;
    }

    void reallocate_storage(const std::size_t size)
    {
        const std::size_t alloc_size = calc_allocation_size(size);
        // If the new capacity is smaller than the size, it is assumed that
        // the objects on the freed region are already correctly destructed.
        mStorage = mAllocator.reallocate(mStorage, alloc_size);
        update_capacity(alloc_size);
        update_elem_array_ptr();
    }

    void release()
    {
        mStorage = nullptr;
        mElements = nullptr;
    }

protected:
    static std::size_t calc_allocation_size(const std::size_t num_elems)
    {
        const auto storage_size = sizeof(T) * num_elems;
        const auto alloc_size = align_up(
            MAX_HEADER_SIZE + storage_size,
            ALLOCATION_SIZE
        );
        return alloc_size;
    }

    void update_capacity(const std::size_t alloc_size)
    {
        assert(storage_initialized());
        // the reminder part not making up an object is dropped
        header()->capacity = (alloc_size - MAX_HEADER_SIZE) / sizeof(T);
    }

    void update_elem_array_ptr()
    {
        assert(storage_initialized());
        // read offset from header to be compatible with configurable header
        // sizes.
        mElements = raw_advance_cast<T>(mStorage, header()->storage_offset);
    }
};
}
