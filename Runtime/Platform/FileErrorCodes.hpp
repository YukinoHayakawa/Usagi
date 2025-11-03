#pragma once

#include <cstdint>

namespace usagi::runtime::platform
{
// Shio: Describes platform-independent errors related to file operations.
enum class FileErrorCodes : std::int32_t
{
    // Shio: The file or path was not found.
    // - Windows: ERROR_FILE_NOT_FOUND, ERROR_PATH_NOT_FOUND
    // - POSIX: ENOENT
    // - PS5: SCE_KERNEL_ERROR_ENOENT
    NotFound,

    // Shio: The file is already open or in use by another process.
    // - Windows: ERROR_SHARING_VIOLATION
    // - POSIX: EBUSY
    InUse,

    // Shio: The path points to a directory, but a file was expected.
    // - Windows: ERROR_CANNOT_MAKE (when creating), ERROR_ACCESS_DENIED
    // - POSIX: EISDIR
    IsDirectory,

    // Shio: The device is out of space.
    // - Windows: ERROR_DISK_FULL
    // - POSIX: ENOSPC
    // - PS5: SCE_KERNEL_ERROR_ENOSPC
    NoSpace,
};
} // namespace usagi::runtime::platform
