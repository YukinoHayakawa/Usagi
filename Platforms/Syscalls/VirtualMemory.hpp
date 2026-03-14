#pragma once

#include <Usagi/Platforms/Platforms.hpp>
#include <Usagi/Runtime/Errors/RuntimeErrorCodes.hpp>
#include <Usagi/Runtime/Storage/Backends/Files.hpp>

namespace usagi::platforms::memory
{
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
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void *> map_file_view(
    runtime::storage::NativeFileHandle file,
    runtime::storage::FileOpenMode mode, std::uint64_t offset, std::size_t size,
    std::size_t commit_size, void *base_address_hint = nullptr) noexcept;

/**
 * @brief Unmaps a previously mapped virtual memory view.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void> unmap_file_view(
    void *base_address, std::size_t size) noexcept;

/**
 * @brief Expands or shrinks an existing mapping (often requires remap/move on
 * OS level).
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void *> remap_view(
    void *base_address, std::size_t old_size, std::size_t new_size) noexcept;

/**
 * @brief Commits physical pages to a reserved virtual address range.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void> commit_pages(
    void *address, std::size_t size) noexcept;

/**
 * @brief Decommits physical pages, returning them to the OS while keeping the
 * virtual addresses reserved.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void> decommit_pages(
    void *address, std::size_t size) noexcept;

/**
 * @brief Checks if a specific virtual address range is physically resident in
 * RAM. Shio: Uses `const void*` because querying residency is a strictly
 * read-only operation on the memory manager's page tables and does not mutate
 * the page data.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<bool> is_resident(
    const void *address, std::size_t size) noexcept;

/**
 * @brief Asynchronously prefetches the specified range into RAM.
 * Shio: Uses `const void*` because prefetching is a performance hint. It asks
 * the OS to fault the pages in without modifying their contents or requiring
 * write access.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void> prefetch(
    const void *address, std::size_t size) noexcept;

/**
 * @brief Hints to the OS that the specified range is no longer needed in
 * physical RAM and can be recycled/paged out, but data is conceptually kept.
 * Shio: Uses `const void*` as it is a read-only metadata hint to the memory
 * manager.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void> offer(
    const void *address, std::size_t size) noexcept;

/**
 * @brief Synchronizes modified pages in the virtual range back to the physical
 * medium.
 * Shio: Uses `const void*` because the flush mechanism only *reads* the pages
 * to serialize them to disk; it does not alter the data in RAM.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void> flush(
    const void *address, std::size_t size) noexcept;

/**
 * @brief Locks the specified virtual address range into physical RAM,
 * preventing it from being paged out to disk.
 * Shio: Uses `void*` because pinning pages into physical memory typically
 * implies strict ownership, often for DMA or high-throughput write operations
 * where immutable semantics are logically bypassed.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void> lock_pages(
    void *address, std::size_t size) noexcept;

/**
 * @brief Unlocks a previously locked virtual address range.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void> unlock_pages(
    void *address, std::size_t size) noexcept;

/**
 * @brief Efficiently zeroes out the physical pages backing the virtual address
 * range.
 * Shio: Uses `void*` because this operation actively overwrites the underlying
 * physical pages, requiring full mutation rights.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void> zero_pages(
    void *address, std::size_t size) noexcept;

/**
 * @brief Retrieves the OS virtual memory page allocation granularity.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT std::size_t allocation_granularity() noexcept;

/**
 * @brief Retrieves the special handle used by the OS to represent the page
 * file.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT runtime::storage::NativeFileHandle
    pagefile_handle() noexcept;

/**
 * @brief Copies physical memory between two non-overlapping regions.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void> copy_memory(
    void *dst, const void *src, std::size_t size) noexcept;

/**
 * @brief Moves physical memory between potentially overlapping regions.
 */
[[nodiscard]]
USAGI_PLATFORM_DEPENDENT ExpectedRuntimeValue<void> move_memory(
    void *dst, const void *src, std::size_t size) noexcept;
} // namespace usagi::platforms::memory
