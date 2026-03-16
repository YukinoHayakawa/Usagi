#include "RegularFileBackend.hpp"

#include <Usagi/Platforms/Syscalls/Files.hpp>
#include <Usagi/Runtime/Errors/Errors.hpp>

#include "MemoryFunctionTable.hpp"

namespace usagi::runtime::storage
{
RegularFileBackend::RegularFileBackend(
    std::filesystem::path path, const FileOpenMode mode,
    const NativeFileHandle handle)
    : mFilePath(std::move(path)), mFileHandle(handle), mMode(mode)
{
}

RegularFileBackend::~RegularFileBackend()
{
    if(mFileHandle != INVALID_FILE_HANDLE)
    {
        platforms::storage::close_file(mFileHandle);
    }
}

RegularFileBackend::RegularFileBackend(RegularFileBackend &&other) noexcept
    : mFilePath(std::move(other.mFilePath))
    , mFileHandle(std::exchange(other.mFileHandle, INVALID_FILE_HANDLE))
    , mMode(other.mMode)
{
}

RegularFileBackend &RegularFileBackend::operator=(
    RegularFileBackend &&other) noexcept
{
    if(this != &other)
    {
        if(mFileHandle != INVALID_FILE_HANDLE)
        {
            // todo error handling
            platforms::storage::close_file(mFileHandle);
        }
        mFilePath   = std::move(other.mFilePath);
        mFileHandle = std::exchange(other.mFileHandle, INVALID_FILE_HANDLE);
        mMode       = other.mMode;
    }
    return *this;
}

ExpectedRuntimeValue<RegularFileBackend> RegularFileBackend::open(
    std::filesystem::path path, // NOLINT(performance-unnecessary-value-param)
    const FileOpenMode mode, const FileShareMode share_mode,
    const FileOpenOptions options)
{
    auto handle_expected =
        platforms::storage::open_file(path, mode, share_mode, options);
    if(!handle_expected.has_value())
    {
        return std::unexpected(handle_expected.error());
    }

    return RegularFileBackend(std::move(path), mode, handle_expected.value());
}

StorageTraits RegularFileBackend::storage_traits() const
{
    // Shio: Query the actual physical traits from the OS/Hardware
    const auto t_expected =
        platforms::storage::query_file_storage_traits(mFileHandle);
    StorageTraits t = t_expected.value_or(StorageTraits { });
    t.is_read_only  = has_all_of(mMode, FileOpenMode::Write) ? 0 : 1;
    return t;
}

std::uint64_t RegularFileBackend::capacity() const
{
    const auto size_expected = platforms::storage::file_size(mFileHandle);
    return size_expected.value_or(0);
}

NativeFileHandle RegularFileBackend::native_handle() const
{
    return mFileHandle;
}

// todo merge with PagefileBackend::create_view
ExpectedRuntimeValue<MemoryView> RegularFileBackend::create_view(
    const std::uint64_t offset, const std::uint64_t size,
    const std::uint64_t commit_size, void *base_address_hint, FileOpenMode mode)
{
    // Resolve "Identical" request to the exact mode the backend was opened
    // with.
    if(mode == FileOpenMode::Identical)
    {
        mode = mMode;
    }

    // Shio: Requested mode cannot exceed the backend's native open mode
    // permissions. e.g., you cannot request ReadWrite mapping if the backend
    // was opened ReadOnly.
    if((static_cast<std::uint8_t>(mode) & static_cast<std::uint8_t>(mMode)) !=
        static_cast<std::uint8_t>(mode))
    {
        return std::unexpected(errors::RuntimeErrorCodes::AccessDenied);
    }

    // todo: move this factory function to VirtualPageManager
    if(!mPageManager)
    {
        // Shio: 1MB bookkeeping view can track ~8 million pages (32GB of
        // memory)
        auto bookkeeping_view = MemoryView::create(
            standard_memory_functions(),
            // todo: heap management
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
        mFileHandle,
        mode,
        storage_traits(),
        mPageManager.get(),
        offset,
        size,
        commit_size,
        base_address_hint);
}

const std::filesystem::path &RegularFileBackend::path() const
{
    return mFilePath;
}

FileOpenMode RegularFileBackend::mode() const
{
    return mMode;
}

std::uint64_t RegularFileBackend::id() const
{
    return platforms::storage::file_id(mFileHandle).value();
}

std::uint64_t RegularFileBackend::last_modification_time() const
{
    return platforms::storage::file_last_modification_time(mFileHandle).value();
}
} // namespace usagi::runtime::storage
