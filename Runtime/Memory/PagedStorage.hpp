#pragma once

#include <Usagi/Concept/Type/Memcpyable.hpp>
#include <Usagi/Library/Container/DynamicArray.hpp>
#include <Usagi/Library/Memory/PoolAllocator.hpp>
#include <Usagi/Runtime/Memory/TypedAllocator.hpp>
#include <Usagi/Runtime/Memory/VmAllocatorFileBacked.hpp>
#include <Usagi/Runtime/Memory/VmAllocatorPagefileBacked.hpp>

namespace usagi
{
namespace detail::paged_storage
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
 * \tparam T
 */
template <Memcpyable T>
class PagedStorageInMemory
    : public PoolAllocator<T, detail::paged_storage::PagefileBackedArray>
{
    using PoolT = PoolAllocator<T, detail::paged_storage::PagefileBackedArray>;
    using StorageT = typename PoolT::StorageT;

public:
    using PoolT::PoolT;

    // Virtual memory reservation
};

/**
 * \brief File-backed storage for Entity Pages and Components.
 * \tparam T
 */
template <Memcpyable T>
class PagedStorageFileBacked
    : public PoolAllocator<T, detail::paged_storage::FileBackedArray>
{
protected:
    using PoolT = PoolAllocator<T, detail::paged_storage::FileBackedArray>;
    using StorageT = typename PoolT::StorageT;

    // file-backed allocator
    constexpr static std::uint16_t MAGIC_CHECK = 0xFA01;
    bool mHeaderInitialized = false;

public:
    PagedStorageFileBacked() = default;

    explicit PagedStorageFileBacked(std::filesystem::path mapped_file)
    {
        init(std::move(mapped_file));
    }

    // todo automatically save database header when an operation is committed
    // return true if it is restoring from existing file
    bool init(std::filesystem::path mapped_file)
    {
        // prepare memory-mapped allocator.
        StorageT::mAllocator.set_backing_file(std::move(mapped_file));

        // if there is already a file, restore metadata
        const bool file_exists = exists(StorageT::mAllocator.path());
        if(file_exists)
        {
            // hopefully the file is not 0-sized or mapping may fail.
            StorageT::rebase(
                StorageT::mAllocator.allocate(MappedFileView::USE_FILE_SIZE),
                false
            );
        }
        StorageT::template push_header<MAGIC_CHECK>(PoolT::mMeta, file_exists);
        mHeaderInitialized = true;

        // otherwise it's a new file. initialization will be handled by the
        // dynamic array. no extra action is needed.
        return file_exists;
    }

    ~PagedStorageFileBacked()
    {
        // save metadata to the header
        if(mHeaderInitialized)
        {
            StorageT::template pop_header<MAGIC_CHECK>(PoolT::mMeta, true);
        }
        // file mapping automatically flushed by base destructor
    }
};
}
