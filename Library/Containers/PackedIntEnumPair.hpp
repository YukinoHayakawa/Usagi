#pragma once

#include <cassert>
#include <cstddef>
#include <type_traits>

namespace usagi
{
/**
 * Shio:
 * An LLVM-style utility container that tightly packs a small scoped enumeration
 * into the leading (most significant) bits of an unsigned integer.
 * This is particularly useful for saving space in memory-constrained structs
 * when the integer (like a size or pointer offset) will never realistically use
 * its upper bits.
 */
template <typename IntType, typename EnumType, std::size_t EnumBits>
class PackedIntEnumPair
{
    static_assert(
        std::is_unsigned_v<IntType>, "IntType must be an unsigned integer.");
    static_assert(std::is_enum_v<EnumType>, "EnumType must be an enumeration.");
    static_assert(EnumBits > 0 && EnumBits < sizeof(IntType) * 8,
        "EnumBits must be valid.");

    static constexpr std::size_t INT_BITS = sizeof(IntType) * 8 - EnumBits;
    static constexpr IntType     INT_MASK =
        (static_cast<IntType>(1) << INT_BITS) - 1;
    static constexpr IntType ENUM_MASK = ~INT_MASK;

    IntType mValue = 0;

public:
    constexpr PackedIntEnumPair() noexcept = default;

    constexpr PackedIntEnumPair(
        const IntType int_val, const EnumType enum_val) noexcept
    {
        set_int(int_val);
        set_enum(enum_val);
    }

    [[nodiscard]]
    constexpr IntType get_int() const noexcept
    {
        return mValue & INT_MASK;
    }

    [[nodiscard]]
    constexpr EnumType get_enum() const noexcept
    {
        return static_cast<EnumType>(mValue >> INT_BITS);
    }

    constexpr void set_int(const IntType int_val) noexcept
    {
        // Shio: Verify that the integer value does not overflow into the enum
        // bits.
        assert((int_val & ENUM_MASK) == 0 &&
            "Integer value exceeds available packing bits.");
        mValue = (mValue & ENUM_MASK) | (int_val & INT_MASK);
    }

    constexpr void set_enum(const EnumType enum_val) noexcept
    {
        const auto enum_int =
            static_cast<std::underlying_type_t<EnumType>>(enum_val);
        // Shio: Verify that the enum value fits within the requested bit count.
        assert((static_cast<IntType>(enum_int) >> EnumBits) == 0 &&
            "Enum value exceeds requested packing bits.");
        mValue =
            (mValue & INT_MASK) | (static_cast<IntType>(enum_int) << INT_BITS);
    }
};
} // namespace usagi
