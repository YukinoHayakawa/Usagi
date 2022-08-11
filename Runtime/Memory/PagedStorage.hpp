#pragma once

#include <Usagi/Concepts/Type/Memcpyable.hpp>
#include <Usagi/Library/Container/DynamicArray.hpp>
#include <Usagi/Library/Memory/PoolAllocator.hpp>
#include <Usagi/Runtime/Memory/TypedAllocator.hpp>
#include <Usagi/Runtime/Memory/VmAllocatorFileBacked.hpp>
#include <Usagi/Runtime/Memory/VmAllocatorPagefileBacked.hpp>

namespace usagi
{
namespace details::paged_storage
{
template <typename T>
using PagefileBackedArray = DynamicArray<
    T,
    TypedAllocator<T, VmAllocatorPagefileBacked>
>;

template <typename T>
using FileBackedArray = DynamicArray<
    T,
    TypedAllocator<T, VmAllocatorFileBacked>
>;
}

/**
 * \brief Storage for Entity Pages and Components that only to be preserved
 * during runtime. Memory space is allocated via reserving and committing
 * virtual memory.
 * \tparam T Element type.
 */
template <Memcpyable T>
class PagedStorageInMemory
    : public PoolAllocator<T, details::paged_storage::PagefileBackedArray>
{
    using PoolT = PoolAllocator<T, details::paged_storage::PagefileBackedArray>;
    using StorageT = typename PoolT::StorageT;

public:
    using PoolT::PoolT;

    // Virtual memory reservation
};

/**
 * \brief File-backed storage for Entity Pages and Components.
 * \tparam T Element type.
 */
template <Memcpyable T>
class PagedStorageFileBacked
    : public PoolAllocator<T, details::paged_storage::FileBackedArray>
{
protected:
    using PoolT = PoolAllocator<T, details::paged_storage::FileBackedArray>;
    using StorageT = typename PoolT::StorageT;
    using AllocatorT = typename StorageT::AllocatorT;

    // file-backed allocator
    constexpr static std::uint64_t MAGIC_CHECK = 0xABBE'5BCEFAFF'FA01;

    std::uint64_t mHeaderOffset = -1;

    struct Header
    {
        std::uint64_t head = details::pool::INVALID_BLOCK;
    };

    std::uint64_t & free_list_head() override
    {
        return StorageT::template header<Header>(mHeaderOffset)->head;
    }

public:
    PagedStorageFileBacked() = default;

    explicit PagedStorageFileBacked(std::filesystem::path mapped_file)
    {
        init(std::move(mapped_file));
    }

    // todo automatically save database header when an operation is committed
    // return true if it is restoring from existing file
    void init(std::filesystem::path mapped_file)
    {
        // prepare memory-mapped allocator.
        AllocatorT alloc;
        alloc.set_backing_file(std::move(mapped_file));
        StorageT::init(std::move(alloc));
        std::tie(mHeaderOffset, std::ignore) =
            StorageT::template init_or_restore_header<
                MAGIC_CHECK, Header>();
    }
};
}
