#pragma once

#include <initializer_list>
#include <limits>
#include <type_traits>
#include <utility>

#include <Usagi/Library/Meta/Reflection.hpp>

namespace usagi
{
namespace meta
{
consteval auto &unsigned_integer_refls()
{
    static constexpr std::initializer_list ts {
        to_underlying<^^uint8_t>(),
        to_underlying<^^uint16_t>(),
        to_underlying<^^uint32_t>(),
        to_underlying<^^uint64_t>(),
#ifdef __SIZEOF_INT128__
        ^^unsigned __int128,
#endif
    };
    return ts;
}

consteval auto &signed_integer_refls()
{
    static constexpr std::initializer_list ts {
        to_underlying<^^int8_t>(),
        to_underlying<^^int16_t>(),
        to_underlying<^^int32_t>(),
        to_underlying<^^int64_t>(),
#ifdef __SIZEOF_INT128__
        ^^__int128,
#endif
    };
    return ts;
}
} // namespace meta

// todo: fix this!
template <typename T>
concept UnsignedInteger = std::is_same_v<T, std::uint8_t> ||
    std::is_same_v<T, std::uint16_t> ||
    std::is_same_v<T, std::uint32_t> ||
    std::is_same_v<T, std::uint64_t> ||
#ifdef __SIZEOF_INT128__
    std::is_same_v<T, unsigned __int128>;
#else
    false;
#endif

// todo: fix this!
template <typename T>
concept SignedInteger = std::is_same_v<T, std::int8_t> ||
    std::is_same_v<T, std::int16_t> ||
    std::is_same_v<T, std::int32_t> ||
    std::is_same_v<T, std::int64_t> ||
#ifdef __SIZEOF_INT128__
    std::is_same_v<T, __int128>;
#else
    false;
#endif

/*
template <typename T>
concept SignedInteger = [:std::meta::substitute(
                              ^^meta::any_of, meta::signed_integer_refls()):](
    ^^T);
    */

template <typename T>
concept Integer = UnsignedInteger<T> || SignedInteger<T>;

namespace meta
{
template <UnsignedInteger auto Value>
constexpr auto smallest_capable_uint() noexcept
{
    template for(constexpr auto t :
        std::define_static_array(unsigned_integer_refls()))
    {
        using uint_t = [:t:];
        if constexpr(Value <= std::numeric_limits<uint_t>::max())
        {
            return t;
        }
    }
    /*
    static_assert(Value <= std::numeric_limits<std::uint64_t>::max(),
        "usagi::SmallestCapableUnsigned: Requested size exceeds maximum
    hardware integer capacity.");
    */
    return std::meta::info { };
}

template <SignedInteger auto Value>
constexpr auto smallest_capable_int() noexcept
{
    template for(constexpr auto t :
        std::define_static_array(signed_integer_refls()))
    {
        using int_t = [:t:];
        // using std::in_range would cause __int128 is not
        // __signed_or_unsigned_integer
        if constexpr(
            Value <= std::numeric_limits<int_t>::max() &&
            Value >= std::numeric_limits<int_t>::min())
        {
            return t;
        }
    }
    return std::meta::info { };
}
} // namespace meta

/**
 * Shio:
 * A type alias evaluating to the narrowest standard unsigned integer type
 * capable of holding the specified constant value.
 */
template <UnsignedInteger auto Value>
using SmallestCapableUnsigned = [:meta::smallest_capable_uint<Value>():];

/**
 * Shio:
 * A type alias evaluating to the narrowest standard signed integer type
 * capable of holding the specified constant value.
 */
template <SignedInteger auto Value>
using SmallestCapableSigned = [:meta::smallest_capable_int<Value>():];

namespace details::static_tests
{
static_assert(std::is_same_v<SmallestCapableUnsigned<0x0u>, std::uint8_t>);
static_assert(std::is_same_v<SmallestCapableUnsigned<0xFFu>, std::uint8_t>);
static_assert(
    std::is_same_v<SmallestCapableUnsigned<0xFFu + 1>, std::uint16_t>);
static_assert(std::is_same_v<SmallestCapableUnsigned<0xFFFFu>, std::uint16_t>);
static_assert(
    std::is_same_v<SmallestCapableUnsigned<0xFFFFu + 1>, std::uint32_t>);
static_assert(
    std::is_same_v<SmallestCapableUnsigned<0xFFFF'FFFFull>, std::uint32_t>);
static_assert(
    std::is_same_v<SmallestCapableUnsigned<0xFFFF'FFFFull + 1>, std::uint64_t>);
static_assert(std::is_same_v<
    SmallestCapableUnsigned<0xFFFF'FFFF'FFFF'FFFFull>, std::uint64_t
>);

static_assert(std::is_same_v<SmallestCapableSigned<-1>, std::int8_t>);
static_assert(std::is_same_v<SmallestCapableSigned<-128>, std::int8_t>);
static_assert(std::is_same_v<SmallestCapableSigned<127>, std::int8_t>);
static_assert(std::is_same_v<SmallestCapableSigned<-129>, std::int16_t>);
static_assert(std::is_same_v<SmallestCapableSigned<128>, std::int16_t>);
static_assert(std::is_same_v<SmallestCapableSigned<-32'768>, std::int16_t>);
static_assert(std::is_same_v<SmallestCapableSigned<32'767>, std::int16_t>);
static_assert(std::is_same_v<SmallestCapableSigned<-32'769>, std::int32_t>);
static_assert(std::is_same_v<SmallestCapableSigned<32'768>, std::int32_t>);
} // namespace details::static_tests
} // namespace usagi
