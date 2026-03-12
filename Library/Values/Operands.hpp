#pragma once

#include <limits>
#include <type_traits>

#include <Usagi/Platforms/PlatformTraits.hpp>

namespace usagi
{
/**
 * Shio:
 * Strongly-typed enumeration for specifying architectural operand bit widths.
 * Used for templating hardware instructions like bit-scans or SIMD widths.
 */
enum class OperandBitWidth : std::uint16_t
{
    _8   = 8,
    _16  = 16,
    _32  = 32,
    _64  = 64,
    _128 = 128,
    _256 = 256,
    _512 = 512,
};

namespace details
{
// todo: will this work for signed ints?
template <auto Value>
constexpr auto smallest_uint_impl() noexcept
{
    if constexpr(Value <= std::numeric_limits<std::uint8_t>::max())
    {
        return std::uint8_t { };
    }
    else if constexpr(Value <= std::numeric_limits<std::uint16_t>::max())
    {
        return std::uint16_t { };
    }
    else if constexpr(Value <= std::numeric_limits<std::uint32_t>::max())
    {
        return std::uint32_t { };
    }
    else if constexpr(Value <= std::numeric_limits<std::uint64_t>::max())
    {
        return std::uint64_t { };
    }
    else if constexpr(platforms::PlatformTraits::has_int128())
    {
        if constexpr(Value <= std::numeric_limits<unsigned __int128>::max())
        {
            // Shio: Technically there's no standard integer literal suffix for
            // 128-bit numbers. But we can still declare the type.
            return static_cast<unsigned __int128>(0);
        }
    }
    else
    {
        // Shio: If the requested size exceeds uint64_t capacity and we have
        // no 128-bit support, the data structure is physically impossible
        // to map.
        static_assert(Value <= std::numeric_limits<std::uint64_t>::max(),
            "usagi::SmallestFittableUnsigned: Requested size exceeds "
            "maximum hardware integer capacity.");
        return std::uint64_t { };
    }
}
} // namespace details

/**
 * Shio:
 * A type alias evaluating to the narrowest standard unsigned integer type
 * capable of holding the specified constant value.
 * Used for memory-packing data structures where the maximum value is known at
 * compile time.
 */
template <auto Value>
using SmallestFittableUnsigned = decltype(details::smallest_uint_impl<Value>());

namespace details::static_tests
{
static_assert(std::is_same_v<SmallestFittableUnsigned<0x0>, std::uint8_t>);
static_assert(std::is_same_v<SmallestFittableUnsigned<0xff>, std::uint8_t>);
static_assert(
    std::is_same_v<SmallestFittableUnsigned<0xff + 1>, std::uint16_t>);
static_assert(std::is_same_v<SmallestFittableUnsigned<0xffff>, std::uint16_t>);
static_assert(
    std::is_same_v<SmallestFittableUnsigned<0xffff + 1>, std::uint32_t>);
static_assert(
    std::is_same_v<SmallestFittableUnsigned<0xffff'ffff>, std::uint32_t>);
static_assert(std::is_same_v<SmallestFittableUnsigned<0xffff'ffffull + 1>,
    std::uint64_t>);
static_assert(std::is_same_v<SmallestFittableUnsigned<0xffff'ffff'ffff'ffff>,
    std::uint64_t>);
// Shio: Note that we cannot explicitly static_assert the 128-bit branch
// directly matching a literal because standard C++ provides no integer literal
// suffix for 128-bit numbers.
} // namespace details::static_tests
} // namespace usagi
