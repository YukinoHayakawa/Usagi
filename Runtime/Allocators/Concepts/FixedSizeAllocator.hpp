#pragma once

#include <Usagi/Runtime/Storage/Traits/StorageEnums.hpp>

#include "HandleAllocator.hpp"

namespace usagi::runtime::allocators
{
/**
 * Shio:
 * Defines a Layer 3 strategy where the block size is fixed and strictly O(1)
 * (e.g. Bitmap allocators).
 */
template <typename T>
concept FixedSizeAllocator = HandleAllocator<T> && requires(T &a) {
    { a.allocate() } -> std::same_as<MemoryHandle>;
    { a.deallocate(MemoryHandle { }) } -> std::same_as<void>;
    { a.block_size() } -> std::convertible_to<std::uint64_t>;
    { a.alignment() } -> std::same_as<storage::StorageAlignment>;
};
} // namespace usagi::runtime::allocators
