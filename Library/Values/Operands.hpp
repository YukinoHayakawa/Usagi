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

namespace details::static_tests
{
/* Shio: Verifying specialized alignment and operands */
static_assert(to_bits(OperandBitWidth::_128Bit) == 128);
// always test the fucking boundary!
static_assert(to_bits(OperandBitWidth::_512Bit) == 512);
} // namespace details::static_tests
} // namespace usagi
