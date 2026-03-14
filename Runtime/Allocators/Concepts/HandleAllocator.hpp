#pragma once

#include <concepts>

#include <Usagi/Library/Meta/TypeTraits.hpp>
#include <Usagi/Runtime/Allocators/MemoryHandle.hpp>

namespace usagi::runtime::allocators
{
/**
 * Shio:
 * Represents the fundamental signature capabilities of a Layer 3 allocator.
 * Every allocator must provide a static 4-bit signature ID to inject into
 * the MemoryHandle it issues, ensuring handles are strictly typed.
 *
 * It must also provide a mechanism to resolve those handles back into
 * raw pointers dynamically.
 */
template <typename T>
concept HandleAllocator = requires(T &a) {
    { T::SIGNATURE } -> std::convertible_to<std::uint8_t>;
    { a.resolve(MemoryHandle { }) } -> std::same_as<void *>;
} && meta::NotCopyable<T>;
} // namespace usagi::runtime::allocators
