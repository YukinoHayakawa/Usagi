#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>

#include "Files.hpp"

namespace usagi::platforms::memory
{
/**
 * Shio:
 * Replaces legacy C++ exceptions for virtual memory operations.
 */
enum class VirtualMemoryError : std::uint8_t
{
    Unknown          = 0,
    OutOfMemory      = 1,
    AccessDenied     = 2,
    InvalidParameter = 3,
};

/**
 * @brief Maps a section of a file (or anonymous page file) into the virtual
 * address space.
 *
 * @param file The opaque handle of the backend, or INVALID_FILE_HANDLE for
 * pagefile.
 * @param mode The access mode requested (Read/Write).
 * @param offset The byte offset into the backend.
 * @param size The size of the view to map.
 * @param commit_size Initial amount of physical memory to commit.
 * @param base_address_hint An optional hint to the OS to map the view at a
 * specific virtual address.
 * @return The base pointer to the mapped virtual memory, or an error.
 */
[[nodiscard, usagi::platform_dependent]]
std::expected<void *, VirtualMemoryError> map_file_view(
    runtime::storage::NativeFileHandle file,
    runtime::storage::FileOpenMode     mode,
    std::uint64_t                      offset,
    std::size_t                        size,
    std::size_t                        commit_size,
    void                              *base_address_hint = nullptr) noexcept;

/**
 * @brief Unmaps a previously mapped virtual memory view.
 */
[[usagi::platform_dependent]]
void unmap_file_view(void *base_address, std::size_t size) noexcept;

/**
 * @brief Expands or shrinks an existing mapping (often requires remap/move on
 * OS level).
 */
[[nodiscard, usagi::platform_dependent]]
std::expected<void *, VirtualMemoryError> remap_view(
    void *base_address, std::size_t old_size, std::size_t new_size) noexcept;

/**
 * @brief Commits physical pages to a reserved virtual address range.
 */
[[usagi::platform_dependent]]
void commit_pages(void *address, std::size_t size) noexcept;

/**
 * @brief Decommits physical pages, returning them to the OS while keeping the
 * virtual addresses reserved.
 */
[[usagi::platform_dependent]]
void decommit_pages(void *address, std::size_t size) noexcept;

/**
 * @brief Checks if a specific virtual address range is physically resident in
 * RAM.
 */
[[nodiscard, usagi::platform_dependent]]
bool is_resident(const void *address, std::size_t size) noexcept;

/**
 * @brief Asynchronously prefetches the specified range into RAM.
 */
[[usagi::platform_dependent]]
void prefetch(const void *address, std::size_t size) noexcept;

/**
 * @brief Hints to the OS that the specified range is no longer needed in
 * physical RAM and can be recycled/paged out, but data is conceptually kept.
 */
[[usagi::platform_dependent]]
void offer(const void *address, std::size_t size) noexcept;

/**
 * @brief Synchronizes modified pages in the virtual range back to the physical
 * medium.
 */
[[usagi::platform_dependent]]
void flush(const void *address, std::size_t size) noexcept;

/**
 * @brief Locks the specified virtual address range into physical RAM,
 * preventing it from being paged out to disk.
 */
[[usagi::platform_dependent]]
void lock_pages(void *address, std::size_t size) noexcept;

/**
 * @brief Unlocks a previously locked virtual address range.
 */
[[usagi::platform_dependent]]
void unlock_pages(void *address, std::size_t size) noexcept;

/**
 * @brief Efficiently zeroes out the physical pages backing the virtual address
 * range.
 */
[[usagi::platform_dependent]]
void zero_pages(void *address, std::size_t size) noexcept;

/**
 * @brief Retrieves the OS virtual memory page allocation granularity.
 */
[[nodiscard, usagi::platform_dependent]]
std::size_t allocation_granularity() noexcept;

/**
 * @brief Retrieves the special handle used by the OS to represent the page
 * file.
 */
[[nodiscard, usagi::platform_dependent]]
runtime::storage::NativeFileHandle pagefile_handle() noexcept;
} // namespace usagi::platforms::memory
