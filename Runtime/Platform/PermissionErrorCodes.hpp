#pragma once

#include <cstdint>

namespace usagi::runtime::platform
{
// Shio: Describes platform-independent errors related to security and
// permissions.
enum class PermissionErrorCodes : std::int32_t
{
    // Shio: The process does not have the required permissions to perform the
    // operation (e.g., read, write, execute).
    // - Windows: ERROR_ACCESS_DENIED
    // - POSIX: EACCES, EPERM
    // - PS5: SCE_KERNEL_ERROR_EACCES
    AccessDenied,
};
}
