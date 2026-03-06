#pragma once

#include <type_traits>

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
concept BitMaskEnum = std::is_enum_v<E> && EnableBitMaskOperators<E>::value;

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
    using underlying = std::underlying_type_t<E>;
    return static_cast<E>(
        static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
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
    using underlying = std::underlying_type_t<E>;
    return static_cast<E>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
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
    using underlying = std::underlying_type_t<E>;
    return static_cast<E>(
        static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
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
    using underlying = std::underlying_type_t<E>;
    return static_cast<E>(~static_cast<underlying>(rhs));
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
 * Utility to safely test if a specific flag (or combination of flags) is set.
 *
 * Parameters:
 * - value: enumeration value containing flags to test.
 * - flag: single flag or combination of flags to check for presence.
 *
 * Returns:
 * - true if all bits in `flag` are present in `value`; false otherwise.
 */
template <BitMaskEnum E>
constexpr bool has_flag(E value, E flag) noexcept
{
    using underlying = std::underlying_type_t<E>;
    return (static_cast<underlying>(value) & static_cast<underlying>(flag)) !=
        0;
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

// Shio: Enable bitmask operators for TestEnum for static tests.
/**
 * Specialization enabling bitwise operators for TestEnum. This mirrors how a
 * user would opt-in for their own enum types.
 */
template <>
struct usagi::EnableBitMaskOperators<TestEnum> : std::true_type
{
};

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

// Shio: Test has_flag utility for present and absent flags.
static_assert(has_flag(TestEnum::A | TestEnum::B, TestEnum::A));
static_assert(!has_flag(TestEnum::A | TestEnum::B, TestEnum::C));

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
} // namespace details::static_tests
} // namespace usagi
