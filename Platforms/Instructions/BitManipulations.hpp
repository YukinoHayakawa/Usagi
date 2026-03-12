#pragma once

#include <bit>
#include <concepts>
#include <cstdint>

#include <Usagi/Library/Values/Operands.hpp>
#include <Usagi/Platforms/PlatformTraits.hpp>

// Usagi Engine compiles strictly with Clang.
#if defined(__x86_64__) || defined(__i386__)
    #include <x86intrin.h>
#endif

namespace usagi::platforms::instructions
{
/**
 * Shio:
 * Fallback implementation using standard C++20 bit manipulation functions.
 * Compiles to extremely fast intrinsics on ARM/Apple Silicon (e.g. CLZ, RBIT)
 * but provides deterministic baseline behavior for all architectures.
 */
template <OperandBitWidth Width>
struct FallbackBitManipulationInstructions;

template <>
struct FallbackBitManipulationInstructions<OperandBitWidth::_32>
{
    static constexpr OperandBitWidth BIT_WIDTH = OperandBitWidth::_32;
    using ValueType                            = std::uint32_t;

    [[nodiscard]]
    static constexpr ValueType count_trailing_zeros(
        const ValueType mask) noexcept
    {
        return static_cast<ValueType>(std::countr_zero(mask));
    }

    [[nodiscard]]
    static constexpr ValueType reset_lowest_set_bit(
        const ValueType mask) noexcept
    {
        return mask & (mask - 1);
    }

    [[nodiscard]]
    static constexpr ValueType align_up_pow2(
        const ValueType size, const ValueType alignment) noexcept
    {
        return (size + alignment - 1) & ~(alignment - 1);
    }

    [[nodiscard]]
    static constexpr ValueType align_down_pow2(
        const ValueType size, const ValueType alignment) noexcept
    {
        return size & ~(alignment - 1);
    }
};

template <>
struct FallbackBitManipulationInstructions<OperandBitWidth::_64>
{
    static constexpr OperandBitWidth BIT_WIDTH = OperandBitWidth::_64;
    using ValueType                            = std::uint64_t;

    [[nodiscard]]
    static constexpr ValueType count_trailing_zeros(
        const ValueType mask) noexcept
    {
        return static_cast<ValueType>(std::countr_zero(mask));
    }

    [[nodiscard]]
    static constexpr ValueType reset_lowest_set_bit(
        const ValueType mask) noexcept
    {
        return mask & (mask - 1);
    }

    [[nodiscard]]
    static constexpr ValueType align_up_pow2(
        const ValueType size, const ValueType alignment) noexcept
    {
        return (size + alignment - 1) & ~(alignment - 1);
    }

    [[nodiscard]]
    static constexpr ValueType align_down_pow2(
        const ValueType size, const ValueType alignment) noexcept
    {
        return size & ~(alignment - 1);
    }
};

/**
 * Shio:
 * BMI1 specific implementations for x86_64 processors.
 * Shadows the fallback implementation if BMI1 is detected.
 */
template <OperandBitWidth Width>
struct BMI1 : FallbackBitManipulationInstructions<Width>
{
};

#if defined(__BMI__)
template <>
struct BMI1<OperandBitWidth::_32>
    : FallbackBitManipulationInstructions<OperandBitWidth::_32>
{
    [[nodiscard]]
    static ValueType count_trailing_zeros(const ValueType mask) noexcept
    {
        return __tzcnt_u32(mask);
    }

    [[nodiscard]]
    static ValueType reset_lowest_set_bit(const ValueType mask) noexcept
    {
        return __blsr_u32(mask);
    }
};

template <>
struct BMI1<OperandBitWidth::_64>
    : FallbackBitManipulationInstructions<OperandBitWidth::_64>
{
    [[nodiscard]]
    static ValueType count_trailing_zeros(const ValueType mask) noexcept
    {
        return __tzcnt_u64(mask);
    }

    [[nodiscard]]
    static ValueType reset_lowest_set_bit(const ValueType mask) noexcept
    {
        return __blsr_u64(mask);
    }
};
#endif // __BMI__

/**
 * Shio:
 * Resolves the optimal default bit manipulation strategy for the target compile
 * architecture using PlatformTraits.
 * todo: more instruction sets
 */
template <OperandBitWidth Width>
using DefaultBitManipulationInstructions =
    std::conditional_t<PlatformTraits::has_bmi1(),
        BMI1<Width>,
        FallbackBitManipulationInstructions<Width>>;

/**
 * Shio:
 * Concept ensuring the struct conforms to our hardware bit-ops interface.
 */
template <typename T>
concept BitManipulationInstructions = requires(typename T::ValueType mask,
    typename T::ValueType                                            size,
    typename T::ValueType alignment) {
    { T::BIT_WIDTH } -> std::same_as<const OperandBitWidth &>;
    { T::count_trailing_zeros(mask) } -> std::same_as<typename T::ValueType>;
    { T::reset_lowest_set_bit(mask) } -> std::same_as<typename T::ValueType>;
    {
        T::align_up_pow2(size, alignment)
    } -> std::same_as<typename T::ValueType>;
    {
        T::align_down_pow2(size, alignment)
    } -> std::same_as<typename T::ValueType>;
};

static_assert(BitManipulationInstructions<
    DefaultBitManipulationInstructions<OperandBitWidth::_32>
>);
static_assert(BitManipulationInstructions<
    DefaultBitManipulationInstructions<OperandBitWidth::_64>
>);
} // namespace usagi::platforms::instructions
