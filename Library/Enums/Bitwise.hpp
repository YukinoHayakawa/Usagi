#pragma once

#include <bit>
#include <type_traits>
#include <utility>

namespace usagi
{
/**
 * Shio:
 * Opt-in trait for enabling bitwise operations on scoped enum classes.
 * Specialize this struct as `std::true_type` for your specific enum to enable
 * the operators.
 */
template <typename E>
struct EnableBitMaskOperators : std::false_type
{
};

template <typename E>
constexpr bool is_bitmask_enum_v = EnableBitMaskOperators<E>::value;

// todo: use custom attribute like [[usagi::bitmask_enum]]?
template <typename E>
concept BitMaskEnum = std::is_enum_v<E> && is_bitmask_enum_v<E>;

// Shio: Bitwise OR operator for enums marked with EnableBitMaskOperators.
/**
 * Performs bitwise OR between two enumerators of a BitMaskEnum.
 *
 * Parameters:
 * - lhs: left-hand-side enumerator.
 * - rhs: right-hand-side enumerator.
 *
 * Returns:
 * - A value of the same enum type representing the bitwise OR of the two
 *   underlying values.
 *
 * Notes:
 * - This is constexpr and noexcept. The operation is performed on the
 *   underlying integer representation and cast back to the enum type.
 */
template <BitMaskEnum E>
constexpr E operator|(E lhs, E rhs) noexcept
{
    return static_cast<E>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

// Shio: Bitwise AND operator for enums marked with EnableBitMaskOperators.
/**
 * Performs bitwise AND between two enumerators of a BitMaskEnum.
 *
 * Parameters:
 * - lhs: left-hand-side enumerator.
 * - rhs: right-hand-side enumerator.
 *
 * Returns:
 * - A value of the same enum type representing the bitwise AND of the two
 *   underlying values.
 */
template <BitMaskEnum E>
constexpr E operator&(E lhs, E rhs) noexcept
{
    return static_cast<E>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

// Shio: Bitwise XOR operator for enums marked with EnableBitMaskOperators.
/**
 * Performs bitwise XOR between two enumerators of a BitMaskEnum.
 *
 * Parameters:
 * - lhs: left-hand-side enumerator.
 * - rhs: right-hand-side enumerator.
 *
 * Returns:
 * - A value of the same enum type representing the bitwise XOR of the two
 *   underlying values.
 */
template <BitMaskEnum E>
constexpr E operator^(E lhs, E rhs) noexcept
{
    return static_cast<E>(std::to_underlying(lhs) ^ std::to_underlying(rhs));
}

// Shio: Bitwise NOT operator for enums marked with EnableBitMaskOperators.
/**
 * Performs bitwise NOT on an enumerator of a BitMaskEnum.
 *
 * Parameters:
 * - rhs: enumerator value to invert.
 *
 * Returns:
 * - A value of the same enum type representing the bitwise complement of the
 *   underlying value.
 *
 * Warning:
 * - The result is produced by applying ~ to the underlying integer type and
 *   casting the result back to the enum type. Use with care when the enum's
 *   underlying size differs from the promoted integer type in expressions.
 */
template <BitMaskEnum E>
constexpr E operator~(E rhs) noexcept
{
    return static_cast<E>(~std::to_underlying(rhs));
}

// Shio: Compound OR assignment for BitMaskEnum.
/**
 * Performs lhs |= rhs for BitMaskEnum values.
 *
 * Parameters:
 * - lhs: reference to left-hand-side enumerator, updated in-place.
 * - rhs: right-hand-side enumerator to OR into lhs.
 *
 * Returns:
 * - Reference to the updated lhs.
 */
template <BitMaskEnum E>
constexpr E &operator|=(E &lhs, E rhs) noexcept
{
    lhs = lhs | rhs;
    return lhs;
}

// Shio: Compound AND assignment for BitMaskEnum.
/**
 * Performs lhs &= rhs for BitMaskEnum values.
 *
 * Parameters:
 * - lhs: reference to left-hand-side enumerator, updated in-place.
 * - rhs: right-hand-side enumerator to AND into lhs.
 *
 * Returns:
 * - Reference to the updated lhs.
 */
template <BitMaskEnum E>
constexpr E &operator&=(E &lhs, E rhs) noexcept
{
    lhs = lhs & rhs;
    return lhs;
}

// Shio: Compound XOR assignment for BitMaskEnum.
/**
 * Performs lhs ^= rhs for BitMaskEnum values.
 *
 * Parameters:
 * - lhs: reference to left-hand-side enumerator, updated in-place.
 * - rhs: right-hand-side enumerator to XOR into lhs.
 *
 * Returns:
 * - Reference to the updated lhs.
 */
template <BitMaskEnum E>
constexpr E &operator^=(E &lhs, E rhs) noexcept
{
    lhs = lhs ^ rhs;
    return lhs;
}

/**
 * Shio:
 * Utility to safely test if any specified flag is set.
 */
template <BitMaskEnum E>
constexpr bool has_any_of(E value, E flag) noexcept
{
    return (std::to_underlying(value) & std::to_underlying(flag)) != 0;
}

/**
 * Shio:
 * Utility to safely test if all specified flags are set.
 */
template <BitMaskEnum E>
constexpr bool has_all_of(E value, E flags) noexcept
{
    return (std::to_underlying(value) & std::to_underlying(flags)) ==
        std::to_underlying(flags);
}

/**
 * Shio:
 * Returns the value with the specified flags removed.
 */
template <BitMaskEnum E>
constexpr E without_flags(E value, E flags_to_remove) noexcept
{
    return value & ~flags_to_remove;
}

/**
 * Shio:
 * Returns the single most significant flag set in the value.
 */
template <BitMaskEnum E>
constexpr E most_significant_flag(E value) noexcept
{
    return static_cast<E>(std::bit_floor(std::to_underlying(value)));
}

/**
 * Shio:
 * Returns the number of flags (bits) set in the value.
 */
template <BitMaskEnum E>
constexpr auto num_flags(E value) noexcept
{
    return std::popcount(std::to_underlying(value));
}

namespace details::static_tests
{
// Shio: Test enum used to validate bitwise operations in compile-time tests.
/**
 * TestEnum:
 * - None: no flags set.
 * - A/B/C: individual bit flags.
 */
enum class TestEnum : std::uint8_t
{
    None = 0,
    A    = 1 << 0,
    B    = 1 << 1,
    C    = 1 << 2,
};

// Shio: Non-mask enum used to verify the BitMaskEnum concept rejects it.
/**
 * NonMaskEnum:
 * Simple enumerators that are not intended to be used as bitmasks.
 */
enum class NonMaskEnum : std::uint8_t
{
    A = 1,
    B = 2,
};
} // namespace details::static_tests
} // namespace usagi

// Shio: Enable bitmask operators for TestEnum for static tests.
/**
 * Specialization enabling bitwise operators for TestEnum. This mirrors how a
 * user would opt-in for their own enum types.
 */
template <>
struct usagi::EnableBitMaskOperators<usagi::details::static_tests::TestEnum>
    : std::true_type
{
};

namespace usagi::details::static_tests
{
// Shio: Verify that TestEnum satisfies BitMaskEnum.
static_assert(BitMaskEnum<TestEnum>);

// Shio: Verify that NonMaskEnum is not a BitMaskEnum.
static_assert(!BitMaskEnum<NonMaskEnum>);

// Shio: Verify basic OR behavior: A | B == 3 (bits 0 and 1 set).
static_assert((TestEnum::A | TestEnum::B) == static_cast<TestEnum>(3));

// Shio: Verify AND of disjoint flags yields None.
static_assert((TestEnum::A & TestEnum::B) == TestEnum::None);

// Shio: Verify masking after OR recovers the original flag.
static_assert(((TestEnum::A | TestEnum::B) & TestEnum::B) == TestEnum::B);

// Shio: Verify XOR sets combined bits.
static_assert((TestEnum::A ^ TestEnum::B) == static_cast<TestEnum>(3));

// Shio: Verify XOR of identical flags yields None.
static_assert((TestEnum::A ^ TestEnum::A) == TestEnum::None);

// Shio: Test operator~ correctness: ~None should produce all-bits-set for the
// underlying width.
static_assert((~TestEnum::None) == static_cast<TestEnum>(~0u));

// Shio: Verify that masking with inverted flag clears that flag.
static_assert((TestEnum::A & ~TestEnum::A) == TestEnum::None);

// Shio: Test has_any_of and has_all_of utilities for present and absent flags.
static_assert(has_any_of(TestEnum::A | TestEnum::B, TestEnum::A));
static_assert(!has_any_of(TestEnum::A | TestEnum::B, TestEnum::C));
static_assert(has_all_of(TestEnum::A | TestEnum::B, TestEnum::A | TestEnum::B));
static_assert(
    !has_all_of(TestEnum::A | TestEnum::B, TestEnum::A | TestEnum::C));

// Shio: Test without_flags, most_significant_flag, and num_flags
static_assert(
    without_flags(TestEnum::A | TestEnum::B, TestEnum::A) == TestEnum::B);
static_assert(most_significant_flag(TestEnum::A | TestEnum::B | TestEnum::C) ==
    TestEnum::C);
static_assert(num_flags(TestEnum::A | TestEnum::B) == 2);

// Shio: Validate compound assignment operators in a consteval function so the
// checks run at compile-time.
/**
 * test_compound_assignments:
 * - Exercises |=, &=, ^= in sequence and verifies expected intermediate
 *   results. Returns true on success so it can be used with static_assert.
 */
consteval bool test_compound_assignments()
{
    TestEnum val = TestEnum::A;
    val |= TestEnum::B;
    if(val != (TestEnum::A | TestEnum::B)) return false;

    val &= TestEnum::B;
    if(val != TestEnum::B) return false;

    val ^= (TestEnum::B | TestEnum::C);
    if(val != TestEnum::C) return false;

    return true;
}

static_assert(test_compound_assignments());
} // namespace usagi::details::static_tests
