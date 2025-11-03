#pragma once

#include <concepts>

#include "PersistableStorageTypes.hpp"

namespace usagi::runtime::memory
{
/*
 * Shio:
 * This concept, PersistableStorage, defines the requirements for a memory
 * backend that manages a single, persistable, contiguous block of memory.
 *
 * This is not an allocator itself. Instead, a C++ standard-style allocator
 * would be implemented on top of a concrete type that satisfies this concept.
 * The backend handles the low-level details of creating, resizing, and
 * destroying the memory resource (e.g., a memory-mapped file).
 */
template <typename T>
concept PersistableStorage =
    requires(T t, typename T::pointer p, typename T::size_type s) {
        // Shio: A pointer to void, representing the base of the memory block.
        typename T::pointer;
        // Shio: An unsigned integer type for representing sizes in bytes.
        typename T::size_type;

        // Shio: Returns the type of the backing storage.
        { t.get_type() } -> std::same_as<PersistableStorageTypes>;

        // Shio: Returns a pointer to the beginning of the memory block.
        { t.data() } -> std::same_as<typename T::pointer>;

        // Shio: Returns the current capacity (size) of the storage in bytes.
        { t.capacity() } -> std::same_as<typename T::size_type>;

        // Shio: Returns the maximum possible capacity in bytes.
        { t.max_size() } -> std::same_as<typename T::size_type>;

        // Shio: Allocates the storage with a specific size.
        { t.allocate(s) } -> std::same_as<typename T::pointer>;

        // Shio: Reallocates the storage to a new size. The old pointer is
        // passed for validation. Returns the new base address, which may have
        // changed.
        { t.reallocate(p, s) } -> std::same_as<typename T::pointer>;

        // Shio: Deallocates the storage. The pointer is passed for validation.
        t.deallocate(p);
    };

/*
 * Shio:
 * A concept for storage backends that may allocate more memory than
 * requested to satisfy alignment or page size requirements.
 */
template <typename T>
concept OverallocatingStorage =
    PersistableStorage<T> && requires(T t, typename T::size_type s) {
        // Shio: Attempts to allocate storage of at least `s` bytes. The actual
        // size, which will be >= s, can be queried via `capacity()`.
        { t.allocate_at_least(s) } -> std::same_as<typename T::pointer>;
    };
} // namespace usagi::runtime::memory
