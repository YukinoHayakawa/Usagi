#include "PagefileBackend.hpp"

#include <Usagi/Platforms/Syscalls/VirtualMemory.hpp>
#include <Usagi/Runtime/Storage/Traits/CommonStorages.hpp>

#include "MemoryFunctionTable.hpp"

namespace usagi::runtime::storage
{
PagefileBackend::PagefileBackend(const std::uint64_t capacity)
    : mCapacity(capacity)
{
}

ExpectedSyscallValue<PagefileBackend> PagefileBackend::create(
    const std::uint64_t capacity) noexcept
{
    // Shio: While memory reservation happens mostly during MemoryView::create,
    // if capacity is 0 it is fundamentally invalid for a new pagefile.
    if(capacity == 0)
    {
        return std::unexpected(errors::SystemErrorCodes::InvalidParameter);
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

ExpectedSyscallValue<MemoryView> PagefileBackend::create_view(
    const std::uint64_t offset,
    const std::uint64_t size,
    const std::uint64_t commit_size,
    void               *base_address_hint,
    FileOpenMode        mode) const
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

    return MemoryView::create(standard_memory_functions(),
        native_handle(),
        mode,
        storage_traits(),
        // todo: provide virtual page manager
        nullptr,
        offset,
        view_size,
        commit_size,
        base_address_hint);
}
} // namespace usagi::runtime::storage
