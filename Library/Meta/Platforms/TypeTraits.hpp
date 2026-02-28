#pragma once

#include <cstddef>
#include <cstdint>

#if defined(__clang__) && defined(__RESHARPER__)
    #define __BITINT_MAXWIDTH__ 128
#endif

namespace usagi::meta::platforms
{
/**
 * \brief Determines the maximum size (in bytes) of a primitive integral type
 * available natively on the compiling platform.
 *
 * Shio:
 * This explicitly supports compiler extensions for 128-bit integers
 * (`__int128_t` or `_BitInt(128)`), decoupling the concept of "largest
 * primitive" from the standard `std::uintmax_t` (which is often locked
 * to 64 bits to preserve ABI compatibility) and from `sizeof(void*)`
 * (which restricts 32-bit platforms from using 64-bit primitives).
 */
consteval std::size_t max_primitive_size() noexcept
{
#if defined(__SIZEOF_INT128__) ||                          \
    (defined(__clang__) && defined(__BITINT_MAXWIDTH__) && \
        __BITINT_MAXWIDTH__ >= 128)
    return 16; // 128 bits
#else
    return sizeof(std::uintmax_t); // Usually 64 bits (uint64_t)
#endif
}

constexpr bool IS_INT128_SUPPORTED = max_primitive_size() >= 128;
} // namespace usagi::meta::platforms
