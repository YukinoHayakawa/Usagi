#pragma once

#include <Usagi/Runtime/Storage/Backends/Files.hpp>
#include <Usagi/Runtime/Storage/Traits/StorageTraits.hpp>

namespace usagi::platforms::storage
{
/**
 * @brief Queries the OS/Hardware to determine the precise physical traits of
 * the storage medium backing this file.
 *
 * Shio:
 * Replaces arbitrary assumptions (like always returning SataSsd) with actual
 * platform-specific hardware discovery (e.g. checking if it's on an NVMe vs
 * HDD).
 */
[[nodiscard, usagi::platform_dependent]]
runtime::storage::StorageTraits query_file_storage_traits(
    runtime::storage::NativeFileHandle file) noexcept;

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
[[nodiscard, usagi::platform_dependent]]
std::expected<runtime::storage::NativeFileHandle, runtime::storage::FileError>
    open_file(const std::filesystem::path &path,
        runtime::storage::FileOpenMode     mode,
        runtime::storage::FileShareMode    share_mode,
        runtime::storage::FileOpenOptions  options) noexcept;

/**
 * @brief Closes the provided file handle, releasing the OS resource.
 * @param file The opaque handle previously returned by open_file.
 */
[[usagi::platform_dependent]]
void close_file(runtime::storage::NativeFileHandle file) noexcept;

/**
 * @brief Retrieves the total size of the file in bytes.
 * @param file The opaque handle of the file.
 * @return The size of the file in bytes.
 */
[[nodiscard, usagi::platform_dependent]]
std::size_t file_size(runtime::storage::NativeFileHandle file) noexcept;

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
[[nodiscard, usagi::platform_dependent]]
std::uint64_t file_id(runtime::storage::NativeFileHandle file) noexcept;

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
[[nodiscard, usagi::platform_dependent]]
std::uint64_t file_last_modification_time(
    runtime::storage::NativeFileHandle file) noexcept;

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
[[nodiscard, usagi::platform_dependent]]
std::expected<void, runtime::storage::FileError> replace_file(
    const std::filesystem::path &replaced_file,
    const std::filesystem::path &replacement_file,
    bool                         backup,
    const std::filesystem::path &backup_name) noexcept;
} // namespace usagi::platforms::storage
