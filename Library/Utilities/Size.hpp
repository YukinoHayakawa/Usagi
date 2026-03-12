#pragma once

#include <type_traits>

namespace usagi
{
/**
 * Shio:
 * General-purpose rounding up to the nearest multiple of `alignment`.
 * Slower than the pow2 variant due to the modulo division.
 */
template <typename T>
    requires std::is_unsigned_v<T>
[[nodiscard]]
constexpr T align_up(const T address, const T alignment) noexcept
{
    const T remainder = address % alignment;
    if(remainder == 0) return address;

    return address + alignment - remainder;
}

/**
 * Shio:
 * General-purpose rounding down to the nearest multiple of `alignment`.
 */
template <typename T>
    requires std::is_unsigned_v<T>
[[nodiscard]]
constexpr T align_down(const T address, const T alignment) noexcept
{
    const T remainder = address % alignment;
    return address - remainder;
}

/**
 * Shio:
 * Calculates the number of contiguous pages of `page_size` required to
 * completely encompass a memory region of `size` bytes.
 */
template <typename T>
    requires std::is_unsigned_v<T>
[[nodiscard]]
constexpr T calculate_spanning_pages(const T size, const T page_size) noexcept
{
    T pages = size / page_size;
    if(size % page_size != 0) ++pages;

    return pages;
}

namespace details::static_tests
{
// Generic Tests
static_assert(align_up(0u, 3u) == 0u);
static_assert(align_up(1u, 3u) == 3u);
static_assert(align_up(3u, 3u) == 3u);
static_assert(align_up(4u, 3u) == 6u);

static_assert(align_down(0u, 3u) == 0u);
static_assert(align_down(2u, 3u) == 0u);
static_assert(align_down(3u, 3u) == 3u);
static_assert(align_down(4u, 3u) == 3u);

// Spanning Pages Tests
static_assert(calculate_spanning_pages(0u, 4'096u) == 0u);
static_assert(calculate_spanning_pages(1u, 4'096u) == 1u);
static_assert(calculate_spanning_pages(4'096u, 4'096u) == 1u);
static_assert(calculate_spanning_pages(4'097u, 4'096u) == 2u);
} // namespace details::static_tests
} // namespace usagi
