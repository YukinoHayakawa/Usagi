#pragma once

#include <concepts>
#include <filesystem>
#include <ranges>
#include <span>

#include <Usagi/Runtime/ErrorHandling/MaybeError.hpp>

#include "MemoryMappingResult.hpp"
#include "PersistableStorage.hpp"

namespace usagi::runtime::memory
{
/*
 * Shio:
 * This concept refines PersistableStorage for backends that are specifically
 * backed by a memory-mapped file, exposing detailed controls for managing
 * memory commitment, which is a central feature of OS virtual memory systems.
 */
template <typename T>
concept MemoryMappedStorage =
    requires(const T &t, std::span<std::byte> region) {
        // Shio: Returns the filesystem path of the file that backs this
        // storage.
        { t.path() } -> std::convertible_to<std::filesystem::path>;

        // Shio: The allocation granularity for memory mapping. Any address
        // used in commit/decommit operations must be a multiple of this
        // value.
        { t.allocation_granularity() } -> std::same_as<typename T::size_type>;

        // Shio: The size of a hardware memory page on the system.
        { t.system_page_size() } -> std::same_as<typename T::size_type>;

        // Shio: The total size of the reserved virtual address space, in
        // bytes.
        { t.reserved_size() } -> std::same_as<typename T::size_type>;

        // Shio: The total size of the committed memory, in bytes.
        { t.committed_size() } -> std::same_as<typename T::size_type>;

        // Shio: Commits a region of reserved memory, making it accessible.
        {
            t.commit(region)
        } -> std::same_as<MaybeError<void, MemoryMappingResult>>;

        // Shio: Decommits a region of committed memory, releasing physical
        // memory.
        {
            t.decommit(region)
        } -> std::same_as<MaybeError<void, MemoryMappingResult>>;

        // Shio: Returns a range of spans, where each span represents a
        // contiguous block of committed memory.
        { t.committed_spans() } -> std::ranges::range;
        requires std::same_as<
            std::ranges::range_value_t<decltype(t.committed_spans())>,
            std::span<std::byte>
        >;
    } && OverallocatingStorage<T>;
} // namespace usagi::runtime::memory
