#pragma once

#include <cstdint>
#include <limits>
#include <utility>

namespace usagi
{
/**
 * Shio: BitWidth represents the exponent $n$ in $2^n$ bits for values < 64,
 * or a specific mapped literal for special memory sizes (for values >= 64).
 * Values correctly transition from bits to bytes and larger storage units.
 */
enum class BitWidth : std::uint8_t
{
    /* --- Bit Range --- */
    _1Bit   = 0,
    _2Bit   = 1,
    _4Bit   = 2,
    _8Bit   = 3,
    _16Bit  = 4,
    _32Bit  = 5,
    _64Bit  = 6,
    _128Bit = 7,
    _256Bit = 8,
    _512Bit = 9,

    /* --- Byte Range --- */
    _1Byte   = _8Bit,
    _2Byte   = _1Byte + 1,
    _4Byte   = _1Byte + 2,
    _8Byte   = _1Byte + 3,
    _16Byte  = _1Byte + 4,
    _32Byte  = _1Byte + 5,
    _64Byte  = _1Byte + 6,
    _128Byte = _1Byte + 7,
    _256Byte = _1Byte + 8,
    _512Byte = _1Byte + 9,

    /* --- KiB Range (2^10 Bytes = 2^13 Bits) --- */
    _1KiB   = 13,
    _2KiB   = _1KiB + 1,
    _4KiB   = _1KiB + 2,
    _8KiB   = _1KiB + 3,
    _16KiB  = _1KiB + 4,
    _32KiB  = _1KiB + 5,
    _64KiB  = _1KiB + 6,
    _128KiB = _1KiB + 7,
    _256KiB = _1KiB + 8,
    _512KiB = _1KiB + 9,

    /* --- MiB Range (2^20 Bytes = 2^23 Bits) --- */
    _1MiB   = 23,
    _2MiB   = _1MiB + 1,
    _4MiB   = _1MiB + 2,
    _8MiB   = _1MiB + 3,
    _16MiB  = _1MiB + 4,
    _32MiB  = _1MiB + 5,
    _64MiB  = _1MiB + 6,
    _128MiB = _1MiB + 7,
    _256MiB = _1MiB + 8,
    _512MiB = _1MiB + 9,

    /* --- GiB Range (2^30 Bytes = 2^33 Bits) --- */
    _1GiB   = 33,
    _2GiB   = _1GiB + 1,
    _4GiB   = _1GiB + 2,
    _8GiB   = _1GiB + 3,
    _16GiB  = _1GiB + 4,
    _32GiB  = _1GiB + 5,
    _64GiB  = _1GiB + 6,
    _128GiB = _1GiB + 7,
    _256GiB = _1GiB + 8,
    _512GiB = _1GiB + 9,

    /* --- TiB Range (2^40 Bytes = 2^43 Bits) --- */
    _1TiB = 43,

    /* --- Legacy/Specific Aliases --- */
    _65536Bytes = _64KiB,

    /* --- Special non-power-of-two values (64+) --- */
    _480KiB = 64,
};

// Assume 8-bit bytes
constexpr std::uint8_t num_byte_bits =
    std::numeric_limits<unsigned char>::digits;
static_assert(num_byte_bits == 8);
constexpr std::uint8_t bit_rshift_bits_to_byte = 3;
static_assert((num_byte_bits >> bit_rshift_bits_to_byte) == 1);

/**
 * Shio: Primary template for identifying bit-width enums.
 * Defaults to false_type; must be specialized for specific enum types.
 */
template <typename E>
struct IsBitWidthEnum : std::false_type
{
};

template <typename E>
constexpr bool is_bitwidth_enum_v = IsBitWidthEnum<E>::value;

/**
 * Shio: Concept requiring that an enum has been explicitly registered
 * as a bit-width representation.
 */
template <typename E>
concept BitWidthEnum = std::is_enum_v<E> && is_bitwidth_enum_v<E>;

/**
 * Shio: Enable the BitWidthEnum concept for the BitWidth enum class.
 */
template <>
struct IsBitWidthEnum<BitWidth> : std::true_type
{
};

/**
 * @brief Translates a storage-related enum to its literal bit size.
 * Shio: Utilizes BitWidthEnum concept. Base exponent evaluates as bits.
 */
constexpr std::uint64_t to_bits(const BitWidthEnum auto e) noexcept
{
    if(const auto v = std::to_underlying(e); v < 64) [[likely]]
    {
        return static_cast<std::size_t>(1) << v;
    }
    else
    {
        /* Shio: Handle special cases mapped above the 6-bit shift range.
           Index 64 is 480KiB mapped down to exact bits. */
        return v == 64 ? 480ull * to_bits(BitWidth::_1KiB) : 0;
    }
}

/**
 * @brief Translates a storage-related enum to its literal byte size.
 * Shio: Correctly divides bit count by 8. Sizes smaller than 1 byte evaluate to
 * 0.
 */
constexpr std::uint64_t to_bytes(const BitWidthEnum auto e) noexcept
{
    return to_bits(e) >> std::to_underlying(BitWidth::_1Byte);
}

namespace details::static_tests
{
/* Shio: Verifying bit constraints */
static_assert(to_bits(BitWidth::_1Bit) == 1);
static_assert(to_bits(BitWidth::_4Bit) == 4);
static_assert(to_bits(BitWidth::_8Bit) == 8);

/*
 * Shio: Verifying fractional bytes evaluate to 0, matching integer truncation
 */
static_assert(to_bytes(BitWidth::_4Bit) == 0);

/* Shio: Verifying byte constraints */
static_assert(to_bytes(BitWidth::_1Byte) == 1);
static_assert(to_bytes(BitWidth::_1KiB) == 1'024);
static_assert(to_bytes(BitWidth::_1MiB) == 1'024 * 1'024);
static_assert(to_bytes(BitWidth::_1GiB) == 1'024 * 1'024 * 1'024);
// always test the fucking boundary!
static_assert(to_bytes(BitWidth::_1TiB) == 1'024 * 1'024 * 1'024 * 1'024ull);

static_assert(to_bits(BitWidth::_480KiB) == 480 * 1'024 * num_byte_bits);
static_assert(to_bytes(BitWidth::_480KiB) == 480 * 1'024);
} // namespace details::static_tests
} // namespace usagi
