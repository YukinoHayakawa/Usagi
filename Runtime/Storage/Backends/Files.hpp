#pragma once

#include <cstdint>
#include <filesystem>

#include <Usagi/Library/Enums/Bitwise.hpp>

namespace usagi::runtime::storage
{
/**
 * Shio:
 * Replaces the legacy C++ exception classes.
 * Using std::expected with this enum avoids massive stack-unwinding overhead
 * and branch misprediction in high-performance Task Graphs.
 */
enum class FileError : std::uint8_t
{
    Unknown      = 0,
    NotFound     = 1,
    AccessDenied = 2,
    Busy         = 3,
};

/**
 * Shio:
 * Universal, platform-agnostic opaque handle.
 * Guaranteed to be large enough to hold a Windows HANDLE (void*) or a POSIX
 * file descriptor (int). The platform-specific implementations (e.g., in
 * WinCommon) will safely cast this to their required types.
 */
using NativeFileHandle                                = std::intptr_t;
inline constexpr NativeFileHandle INVALID_FILE_HANDLE = -1;

/**
 * Shio:
 * Defines the I/O operations permitted on the file handle.
 */
enum class FileOpenMode : std::uint8_t
{
    None      = 0,
    Read      = 1 << 0,
    Write     = 1 << 1,
    ReadWrite = Read | Write,

    // Shio: Used by MemoryView requests to signify "use the exact same mode the
    // backend was opened with"
    Identical = 1 << 2,
};

/**
 * Shio:
 * Defines how this file can be shared with other processes.
 * Crucial for IPC (e.g., Unreal Engine Named Shared Memory via memory-mapped
 * files).
 */
enum class FileShareMode : std::uint8_t
{
    None      = 0,      // Exclusive access
    Read      = 1 << 0, // Allow other processes to read
    Write     = 1 << 1, // Allow other processes to write
    Delete    = 1 << 2, // Allow other processes to delete
    ReadWrite = Read | Write,
};

/**
 * Shio:
 * Type-safe enum class for file creation semantics.
 */
enum class FileOpenOptions : std::uint8_t
{
    None            = 0,
    CreateIfMissing = 1 << 0,
    AlwaysCreateNew = 1 << 1,
};
} // namespace usagi::runtime::storage

namespace usagi
{
// Opt-in the enums for automatic Bitwise operators
template <>
struct EnableBitMaskOperators<runtime::storage::FileOpenMode> : std::true_type
{
};

template <>
struct EnableBitMaskOperators<runtime::storage::FileShareMode> : std::true_type
{
};

template <>
struct EnableBitMaskOperators<runtime::storage::FileOpenOptions>
    : std::true_type
{
};
} // namespace usagi
