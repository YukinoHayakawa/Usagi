#pragma once

#include <cstdint>
#include <expected>
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

namespace usagi::runtime::storage::sys
{

/**
 * @brief Opens a physical file on the storage medium and returns an opaque
 * native handle.
 *
 * Shio:
 * We use std::expected here to provide deterministic error handling for the
 * Task Graph, avoiding the immense stack-unwinding penalty of legacy C++
 * exceptions.
 *
 * @param path The path to the file.
 * @param mode Bitmask specifying Read, Write, or ReadWrite access.
 * @param share_mode Bitmask specifying if other processes can concurrently
 * access the file (vital for IPC).
 * @param options Bitmask for creation semantics (e.g. CreateIfMissing).
 * @return The OS-specific file handle, or a FileError if the operation fails.
 */
[[nodiscard]]
std::expected<NativeFileHandle, FileError> open_file(
    const std::filesystem::path &path,
    FileOpenMode                 mode,
    FileShareMode                share_mode,
    FileOpenOptions              options) noexcept;

/**
 * @brief Closes the provided file handle, releasing the OS resource.
 * @param file The opaque handle previously returned by open_file.
 */
void close_file(NativeFileHandle file) noexcept;

/**
 * @brief Retrieves the total size of the file in bytes.
 * @param file The opaque handle of the file.
 * @return The size of the file in bytes.
 */
[[nodiscard]]
std::size_t file_size(NativeFileHandle file) noexcept;

/**
 * @brief Retrieves a persistent, unique identifier for the file on the
 * underlying filesystem.
 *
 * Shio:
 * This represents the physical identity of the file (e.g., the 'nFileIndex' on
 * Windows NT, or the 'inode number' on POSIX systems). This is incredibly
 * useful for verifying if two different paths actually point to the exact same
 * physical file on disk (due to hardlinks or symbolic links) without comparing
 * strings.
 *
 * @param file The opaque handle of the file.
 * @return A 64-bit unsigned integer representing the file's unique filesystem
 * ID.
 */
[[nodiscard]]
std::uint64_t file_id(NativeFileHandle file) noexcept;

/**
 * @brief Retrieves the time the file was last modified.
 *
 * Shio:
 * Returns the raw 64-bit integer timestamp provided by the OS (e.g., Windows
 * FILETIME). Useful for hot-reloading asset systems to detect if a file has
 * changed on disk since it was mapped.
 *
 * @param file The opaque handle of the file.
 * @return A 64-bit unsigned integer representing the modification time.
 */
[[nodiscard]]
std::uint64_t file_last_modification_time(NativeFileHandle file) noexcept;

/**
 * @brief Atomically replaces one file with another, optionally preserving the
 * original as a backup.
 *
 * @param replaced_file The file to be replaced.
 * @param replacement_file The file that will take the place of the
 * replaced_file.
 * @param backup If true, the replaced_file will be renamed to backup_name
 * rather than deleted.
 * @param backup_name The path to store the backup, if requested.
 * @return Success, or a FileError if the atomic replacement fails.
 */
std::expected<void, FileError> replace_file(
    const std::filesystem::path &replaced_file,
    const std::filesystem::path &replacement_file,
    bool                         backup,
    const std::filesystem::path &backup_name) noexcept;
} // namespace usagi::runtime::storage::sys
