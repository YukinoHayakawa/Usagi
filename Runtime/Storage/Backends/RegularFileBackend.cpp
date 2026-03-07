#include "RegularFileBackend.hpp"

#include <Usagi/Platforms/Syscalls/Files.hpp>

#include "MemoryFunctionTable.hpp"

namespace usagi::runtime::storage
{
RegularFileBackend::RegularFileBackend(std::filesystem::path path,
    const FileOpenMode                                       mode,
    const NativeFileHandle                                   handle)
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

std::expected<RegularFileBackend, FileError> RegularFileBackend::open(
    std::filesystem::path path, // NOLINT(performance-unnecessary-value-param)
    const FileOpenMode    mode,
    const FileShareMode   share_mode,
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
    StorageTraits t =
        platforms::storage::query_file_storage_traits(mFileHandle);
    t.is_read_only = has_flag(mMode, FileOpenMode::Write) ? 0 : 1;
    return t;
}

std::uint64_t RegularFileBackend::capacity() const
{
    return platforms::storage::file_size(mFileHandle);
}

NativeFileHandle RegularFileBackend::native_handle() const
{
    return mFileHandle;
}

std::expected<MemoryView, platforms::memory::VirtualMemoryError>
    RegularFileBackend::create_view(const std::uint64_t offset,
        const std::uint64_t                             size,
        const std::uint64_t                             commit_size,
        void                                           *base_address_hint,
        FileOpenMode                                    mode) const
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
        return std::unexpected(
            platforms::memory::VirtualMemoryError::AccessDenied);
    }

    return MemoryView::create(standard_memory_functions(),
        mFileHandle,
        mode,
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
    return platforms::storage::file_id(mFileHandle);
}

std::uint64_t RegularFileBackend::last_modification_time() const
{
    return platforms::storage::file_last_modification_time(mFileHandle);
}
} // namespace usagi::runtime::storage
