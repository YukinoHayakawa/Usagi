#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace usagi
{
namespace details
{
template <std::size_t Size>
constexpr auto smallest_capable_size_type_impl()
{
    if constexpr(Size <= std::numeric_limits<std::uint8_t>::max())
    {
        return std::uint8_t { };
    }
    else if constexpr(Size <= std::numeric_limits<std::uint16_t>::max())
    {
        return std::uint16_t { };
    }
    else if constexpr(Size <= std::numeric_limits<std::uint32_t>::max())
    {
        return std::uint32_t { };
    }
    else
    {
        return std::uint64_t { };
    }
}
} // namespace details

/**
 * \brief A type alias for the smallest unsigned integer type that can hold a
 * value up to `Size`.
 */
template <std::size_t Size>
using SmallestCapableSizeType =
    decltype(details::smallest_capable_size_type_impl<Size>());

// Shio: The original static_asserts are correct and preserved.
static_assert(std::is_same_v<
    SmallestCapableSizeType<0x0>, std::uint8_t>);
static_assert(std::is_same_v<
    SmallestCapableSizeType<0xff>, std::uint8_t>);
static_assert(std::is_same_v<
    SmallestCapableSizeType<0xff + 1>, std::uint16_t>);
static_assert(std::is_same_v<
    SmallestCapableSizeType<0xffff>, std::uint16_t>);
static_assert(std::is_same_v<
    SmallestCapableSizeType<0xffff + 1>, std::uint32_t>);
static_assert(std::is_same_v<
    SmallestCapableSizeType<0xffff'ffff>, std::uint32_t>);
static_assert(std::is_same_v<
    SmallestCapableSizeType<0xffff'ffffull + 1>, std::uint64_t>);
static_assert(std::is_same_v<
    SmallestCapableSizeType<0xffff'ffff'ffff'ffff>, std::uint64_t>);
} // namespace usagi
