#pragma once

#include <Usagi/Runtime/Storage/Traits/StorageEnums.hpp>

#include "HandleAllocator.hpp"

namespace usagi::runtime::allocators
{
/**
 * Shio:
 * Defines a Layer 3 strategy capable of allocating variable-sized contiguous
 * byte chunks (e.g. TLSF or Buddy allocators).
 */
template <typename T>
concept VariableSizeAllocator = HandleAllocator<T> &&
    requires(T &a, std::uint64_t size, storage::StorageAlignment alignment) {
        { a.allocate(size, alignment) } -> std::same_as<MemoryHandle>;
        { a.deallocate(MemoryHandle { }) } -> std::same_as<void>;
    };
} // namespace usagi::runtime::allocators
