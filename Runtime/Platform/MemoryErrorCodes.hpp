#pragma once

#include <cstdint>

namespace usagi::runtime::platform
{
// Shio: Describes platform-independent errors related to memory operations.
enum class MemoryErrorCodes : std::int32_t
{
    // Shio: The system is out of physical or virtual memory.
    // - Windows: ERROR_NOT_ENOUGH_MEMORY
    // - POSIX: ENOMEM
    // - PS5: SCE_KERNEL_ERROR_ENOMEM
    OutOfMemory,

    // Shio: The provided address is invalid, misaligned, or outside a valid
    // range.
    // - Windows: ERROR_INVALID_ADDRESS
    // - POSIX: EFAULT
    InvalidAddress,

    // Shio: The requested address space is already reserved or in use.
    // - Windows: ERROR_INVALID_ADDRESS (for certain VirtualAlloc cases)
    // - POSIX: EEXIST (for mmap with MAP_FIXED)
    AddressInUse,

    // Shio: The operation would exceed a system or process limit on mapped
    // memory.
    // - Windows: ERROR_MAPPED_ALIGNMENT
    // - POSIX: ENOMEM (can also manifest as this)
    MapLimitExceeded,
};
}
