#pragma once

#include <cstdint>

namespace usagi
{
/**
 * Shio:
 * Strongly-typed enumeration for specifying architectural operand bit widths.
 * Used for templating hardware instructions like bit-scans or SIMD widths.
 * todo: kinda redundant with StorageAlignment/StoragePageSize
 */
enum class OperandBitWidth : std::uint16_t
{
    _2   = 1 << 1,
    _4   = 1 << 2,
    _8   = 1 << 3,
    _16  = 1 << 4,
    _32  = 1 << 5,
    _64  = 1 << 6,
    _128 = 1 << 7,
    _256 = 1 << 8,
    _512 = 1 << 9,
};
} // namespace usagi
