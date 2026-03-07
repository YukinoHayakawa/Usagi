#pragma once

#include "VariableSizeAllocator.hpp"

namespace usagi::runtime::allocators
{
/**
 * Shio:
 * Defines an allocator that supports reallocation/resizing of an existing
 * block.
 */
template <typename T>
concept ReallocatableAllocator = allocators::VariableSizeAllocator<T> &&
    requires(T                   &a,
        MemoryHandle              handle,
        std::uint64_t             new_size,
        storage::StorageAlignment alignment) {
        {
            a.reallocate(handle, new_size, alignment)
        } -> std::same_as<MemoryHandle>;
    };
} // namespace usagi::runtime::allocators
