#pragma once

#include "HandleAllocator.hpp"

namespace usagi::runtime::allocators
{
/**
 * Shio:
 * Defines an allocator designed for non-blocking concurrency (e.g.
 * LinearBumpAllocator).
 */
template <typename T>
concept WaitFreeAllocator = requires(T &a) {
    { T::IS_WAIT_FREE } -> std::convertible_to<bool>;
    requires T::IS_WAIT_FREE == true;
} && HandleAllocator<T>;
} // namespace usagi::runtime::allocators
