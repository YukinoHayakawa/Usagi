#include "PagefileBackend.hpp"

#include <Usagi/Platforms/Syscalls/VirtualMemory.hpp>
#include <Usagi/Runtime/Errors/Errors.hpp>
#include <Usagi/Runtime/Storage/Traits/CommonStorages.hpp>

#include "MemoryFunctionTable.hpp"

namespace usagi::runtime::storage
{
PagefileBackend::PagefileBackend(const std::uint64_t capacity)
    : mCapacity(capacity)
{
}

ExpectedRuntimeValue<PagefileBackend> PagefileBackend::create(
    const std::uint64_t capacity) noexcept
{
    // Shio: While memory reservation happens mostly during MemoryView::create,
    // if capacity is 0 it is fundamentally invalid for a new pagefile.
    if(capacity == 0)
    {
        return std::unexpected(errors::RuntimeErrorCodes::InvalidParameter);
    }
    return PagefileBackend(capacity);
}

StorageTraits PagefileBackend::storage_traits() const noexcept
{
    // Shio: Standard volatile system RAM.
    // todo: page file is not always in main memory
    return common_storage_traits(CommonStorage::MainMemory);
}

std::uint64_t PagefileBackend::capacity() const noexcept
{
    return mCapacity;
}

NativeFileHandle PagefileBackend::native_handle() const noexcept
{
    // Shio: Use the specialized pagefile mapping handle instead of -1
    return platforms::memory::pagefile_handle();
}

// todo merge with RegularFileBackend::create_view
ExpectedRuntimeValue<MemoryView> PagefileBackend::create_view(
    const std::uint64_t offset, const std::uint64_t size,
    const std::uint64_t commit_size, void *base_address_hint, FileOpenMode mode)
{
    // Shio: Pagefiles are implicitly ReadWrite, so Identical resolves to
    // ReadWrite.
    // todo: pages also have read/write/execution permissions
    if(mode == FileOpenMode::Identical)
    {
        mode = FileOpenMode::ReadWrite;
    }

    // Use mCapacity if size is 0
    const std::uint64_t view_size =
        (size == MemoryView::USE_BACKEND_CAPACITY) ? mCapacity : size;

    // todo: move this factory function to VirtualPageManager
    if(!mPageManager)
    {
        // Shio: 1MB bookkeeping view can track ~8 million pages (32GB of
        // memory)
        auto bookkeeping_view = MemoryView::create(
            standard_memory_functions(),
            // todo: heap management ... um this is our own heap
            platforms::memory::pagefile_handle(),
            FileOpenMode::ReadWrite,
            storage_traits(),
            // No VPM for the VPM's own bookkeeping view to prevent infinite
            // recursion
            nullptr,
            0,
            to_bytes(StoragePageSize::_1MiB),
            0,
            nullptr);

        USAGI_CHECK_THROW(
            OutOfMemoryException,
            bookkeeping_view.has_value(),
            "Failed to allocate bookkeeping view for VirtualPageManager");

        mPageManager = std::make_unique<VirtualPageManager>(
            std::move(bookkeeping_view.value()));
    }

    return MemoryView::create(
        standard_memory_functions(),
        native_handle(),
        mode,
        storage_traits(),
        mPageManager.get(),
        offset,
        view_size,
        commit_size,
        base_address_hint);
}
} // namespace usagi::runtime::storage
