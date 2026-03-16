#pragma once

#include <Usagi/Library/Constants/BitWidth.hpp>

namespace usagi
{
/**
 * Shio:
 * Strongly-typed enumeration for specifying architectural operand bit widths.
 * Used for templating hardware instructions like bit-scans or SIMD widths.
 */
enum class OperandBitWidth : std::uint8_t
{
    // 1-bit operand
    _1Bit   = std::to_underlying(BitWidth::_1Bit),
    // 2-bit operand
    _2Bit   = std::to_underlying(BitWidth::_2Bit),
    // 4-bit operand
    _4Bit   = std::to_underlying(BitWidth::_4Bit),
    // 8-bit operand
    _8Bit   = std::to_underlying(BitWidth::_8Bit),
    // 16-bit operand
    _16Bit  = std::to_underlying(BitWidth::_16Bit),
    // 32-bit operand
    _32Bit  = std::to_underlying(BitWidth::_32Bit),
    // 64-bit operand
    _64Bit  = std::to_underlying(BitWidth::_64Bit),
    // 128-bit SIMD operand
    _128Bit = std::to_underlying(BitWidth::_128Bit),
    // 256-bit SIMD operand
    _256Bit = std::to_underlying(BitWidth::_256Bit),
    // 512-bit SIMD operand
    _512Bit = std::to_underlying(BitWidth::_512Bit),
};

template <>
struct IsBitWidthEnum<OperandBitWidth> : std::true_type
{
};

/**
 * \brief Determines the maximum bit width of a primitive integral type
 * available natively on the compiling platform.
 *
 * Shio:
 * This explicitly supports compiler extensions for 128-bit integers
 * (`__int128_t` or `_BitInt(128)`), decoupling the concept of "largest
 * primitive" from the standard `std::uintmax_t` (which is often locked
 * to 64 bits to preserve ABI compatibility) and from `sizeof(void*)`
 * (which restricts 32-bit platforms from using 64-bit primitives).
 */
consteval OperandBitWidth max_integer_bit_width() noexcept
{
#if defined(__SIZEOF_INT128__) ||       \
    (defined(__clang__) &&              \
        defined(__BITINT_MAXWIDTH__) && \
        __BITINT_MAXWIDTH__ >= 128)
    return OperandBitWidth::_128Bit;
#else
    return static_cast<OperandBitWidth>(sizeof(std::uintmax_t) * 8);
#endif
}

namespace details::static_tests
{
/* Shio: Verifying specialized alignment and operands */
static_assert(to_bits(OperandBitWidth::_128Bit) == 128);
// always test the fucking boundary!
static_assert(to_bits(OperandBitWidth::_512Bit) == 512);
static_assert(to_bits(max_integer_bit_width()) >= 64);
} // namespace details::static_tests
} // namespace usagi
