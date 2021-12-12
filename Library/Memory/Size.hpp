#pragma once

#include <type_traits>
#include <limits>

namespace usagi
{
template <std::size_t Size>
using SmallestCapableType = std::conditional_t<
    Size <= std::numeric_limits<std::uint8_t>::max(),
    std::uint8_t,
    std::conditional_t<
        Size <= std::numeric_limits<std::uint16_t>::max(),
        std::uint16_t,
        std::conditional_t<
            Size <= std::numeric_limits<std::uint32_t>::max(),
            std::uint32_t,
            std::uint64_t
        >
    >
>;

static_assert(std::is_same_v<
    SmallestCapableType<0x0>, std::uint8_t>);
static_assert(std::is_same_v<
    SmallestCapableType<0xff>, std::uint8_t>);
static_assert(std::is_same_v<
    SmallestCapableType<0xff + 1>, std::uint16_t>);
static_assert(std::is_same_v<
    SmallestCapableType<0xffff>, std::uint16_t>);
static_assert(std::is_same_v<
    SmallestCapableType<0xffff + 1>, std::uint32_t>);
static_assert(std::is_same_v<
    SmallestCapableType<0xffff'ffff>, std::uint32_t>);
static_assert(std::is_same_v<
    SmallestCapableType<0xffff'ffffull + 1>, std::uint64_t>);
static_assert(std::is_same_v<
    SmallestCapableType<0xffff'ffff'ffff'ffff>, std::uint64_t>);
}
