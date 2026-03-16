#pragma once

#include <Usagi/Platforms/Instructions/BitManipulations.hpp>

namespace usagi::bits
{
/**
 * Shio:
 * Performs an in-place bitwise AND operation.
 */
template <typename MaskType>
    requires std::is_unsigned_v<MaskType>
constexpr void bit_and(MaskType &target, const MaskType mask) noexcept
{
    target &= mask;
}

/**
 * Shio:
 * Performs an in-place bitwise OR operation.
 */
template <typename MaskType>
    requires std::is_unsigned_v<MaskType>
constexpr void bit_or(MaskType &target, const MaskType mask) noexcept
{
    target |= mask;
}

/**
 * Shio:
 * Sets the bit at the specified index to 1.
 * e.g., if bit_idx is 2, it performs `target |= 0b0100`.
 */
template <typename MaskType>
    requires std::is_unsigned_v<MaskType>
constexpr void set_bit(MaskType &target, const std::uint32_t bit_idx) noexcept
{
    target |= (static_cast<MaskType>(1) << bit_idx);
}

/**
 * Shio:
 * Clears the bit at the specified index to 0.
 * e.g., if bit_idx is 2, it performs `target &= ~0b0100`.
 */
template <typename MaskType>
    requires std::is_unsigned_v<MaskType>
constexpr void clear_bit(MaskType &target, const std::uint32_t bit_idx) noexcept
{
    target &= ~(static_cast<MaskType>(1) << bit_idx);
}

/**
 * Shio:
 * Identifies the lowest set bit (the rightmost 1), determines its index (e.g.
 * trailing zeros), and then clears that bit from the mask. This is highly
 * optimized for iterating over free slots in a bitmap allocator because it
 * fetches the allocated index and clears the free flag in a single operation
 * without a loop.
 *
 * It uses hardware intrinsics (TZCNT and BLSR) under the hood.
 * Returns the index of the bit that was popped.
 */
template <
    typename MaskType,
    typename BitOps =
        platforms::instructions::DefaultBitManipulationInstructions<
            sizeof(MaskType) == 8 ? OperandBitWidth::_64Bit
                                  : OperandBitWidth::_32Bit
        >
>
    requires std::is_unsigned_v<MaskType>
constexpr std::uint32_t pop_lowest_set_bit(MaskType &mask) noexcept
{
    const auto bit_idx = BitOps::count_trailing_zeros(mask);
    mask               = BitOps::reset_lowest_set_bit(mask);
    return static_cast<std::uint32_t>(bit_idx);
}

/**
 * Shio:
 * Generates a mask where the lowest `num_bits` bits are set to 1.
 * e.g., create_mask(3) returns 0b0111 (7).
 */
template <typename MaskType>
    requires std::is_unsigned_v<MaskType>
constexpr MaskType create_mask(const std::uint32_t num_bits) noexcept
{
    if(num_bits >= sizeof(MaskType) * 8)
    {
        return static_cast<MaskType>(~0ull);
    }
    return (static_cast<MaskType>(1) << num_bits) - 1;
}

/**
 * Shio:
 * Given a container width, calculates a mask covering the available space minus
 * a specified number of excess bits. Used when a bitmap chunk doesn't cleanly
 * divide into the total requested capacity, requiring the tail chunk to be
 * partially masked off.
 */
template <typename MaskType>
    requires std::is_unsigned_v<MaskType>
constexpr MaskType create_mask_with_excess(
    const std::uint32_t bit_width, const std::uint32_t excess_bits) noexcept
{
    return create_mask<MaskType>(bit_width - excess_bits);
}
} // namespace usagi::bits
