#pragma once

#include <bit>
#include <concepts>

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
 * Concept ensuring the struct conforms to our hardware bit-ops interface.
 */
template <typename T, typename ValueType = T::ValueType>
concept BitManipulationInstructions = requires(
    ValueType mask, ValueType size, ValueType alignment, ValueType control,
    ValueType src, ValueType index, ValueType a, ValueType b, ValueType &high) {
    // bit width of the operand type
    { T::BIT_WIDTH } -> std::convertible_to<OperandBitWidth>;
    // count least-significant zero bits (TZCNT/BMI1)
    { T::count_trailing_zeros(mask) } -> std::same_as<ValueType>;
    // clear the lowest set bit (BLSR/BMI1)
    { T::reset_lowest_set_bit(mask) } -> std::same_as<ValueType>;
    // round up to the next alignment boundary
    { T::align_up_pow2(size, alignment) } -> std::same_as<ValueType>;
    // round down to the alignment boundary
    { T::align_down_pow2(size, alignment) } -> std::same_as<ValueType>;
    // count set bits (POPCNT/ABM)
    { T::popcount(mask) } -> std::same_as<ValueType>;
    // count most-significant zero bits (LZCNT/ABM)
    { T::count_leading_zeros(mask) } -> std::same_as<ValueType>;
    // extract a bit field (BEXTR/BMI1)
    { T::bit_field_extract(src, control) } -> std::same_as<ValueType>;
    // compress bits selected by mask into low bits (PEXT/BMI2)
    { T::parallel_bits_extract(src, mask) } -> std::same_as<ValueType>;
    // expand low bits into positions selected by mask (PDEP/BMI2)
    { T::parallel_bits_deposit(src, mask) } -> std::same_as<ValueType>;
    // clear bits above a given index (BZHI/BMI2)
    { T::bit_zone_clear(src, index) } -> std::same_as<ValueType>;
    // full unsigned multiply; returns low half, writes high half (MULX/BMI2)
    {
        T::unsigned_multiply_without_affecting_flags(a, b, high)
    } -> std::same_as<ValueType>;
};

/**
 * Shio:
 * Fallback implementation using standard C++20 bit manipulation functions.
 * Compiles to extremely fast intrinsics on ARM/Apple Silicon (e.g. CLZ, RBIT)
 * but provides deterministic baseline behavior for all architectures.
 */
template <OperandBitWidth Width>
    requires (Width == OperandBitWidth::_32 || Width == OperandBitWidth::_64)
struct FallbackBitManipulationInstructions
{
    static constexpr OperandBitWidth BIT_WIDTH = Width;
    using ValueType                            = std::conditional_t<
        Width == OperandBitWidth::_32, std::uint32_t, std::uint64_t
    >;

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
        // mask & (mask - 1)
        return mask & (mask - 1);
    }

    [[nodiscard]]
    static constexpr ValueType align_up_pow2(
        const ValueType size, const ValueType alignment) noexcept
    {
        // (size + alignment - 1) & ~(alignment - 1)
        return (size + alignment - 1) & ~(alignment - 1);
    }

    [[nodiscard]]
    static constexpr ValueType align_down_pow2(
        const ValueType size, const ValueType alignment) noexcept
    {
        // size & ~(alignment - 1)
        return size & ~(alignment - 1);
    }

    [[nodiscard]]
    static constexpr ValueType popcount(const ValueType mask) noexcept
    {
        return static_cast<ValueType>(std::popcount(mask));
    }

    [[nodiscard]]
    static constexpr ValueType count_leading_zeros(
        const ValueType mask) noexcept
    {
        return static_cast<ValueType>(std::countl_zero(mask));
    }

    [[nodiscard]]
    static constexpr ValueType bit_field_extract(
        const ValueType src, const ValueType control) noexcept
    {
        // Software fallback for BEXTR (BMI1)
        ValueType start = control & 0xFF;
        ValueType len   = (control >> 8) & 0xFF;
        if(start >= static_cast<ValueType>(Width)) return 0;
        if(len > static_cast<ValueType>(Width) - start)
            len = static_cast<ValueType>(Width) - start;
        if(len == 0) return 0;
        return (src >> start) & ((ValueType(1) << len) - 1);
    }

    [[nodiscard]]
    static constexpr ValueType parallel_bits_extract(
        const ValueType src, const ValueType mask) noexcept
    {
        // Software fallback for PEXTR (BMI2)
        ValueType res = 0;
        ValueType bit = 1;
        for(ValueType m = mask, s = src; m; m >>= 1, s >>= 1)
        {
            if(m & 1)
            {
                if(s & 1) res |= bit;
                bit <<= 1;
            }
        }
        return res;
    }

    [[nodiscard]]
    static constexpr ValueType parallel_bits_deposit(
        const ValueType src, const ValueType mask) noexcept
    {
        // Software fallback for PDEP (BMI2)
        ValueType res = 0;
        ValueType s   = src;
        for(ValueType m = mask, bit = 1; m; m >>= 1, bit <<= 1)
        {
            if(m & 1)
            {
                if(s & 1) res |= bit;
                s >>= 1;
            }
        }
        return res;
    }

    [[nodiscard]]
    static constexpr ValueType bit_zone_clear(
        const ValueType src, const ValueType index) noexcept
    {
        // Software fallback for BZHI (BMI2)
        if(index >= static_cast<ValueType>(Width)) return src;
        return src & ((ValueType(1) << index) - 1);
    }

    [[nodiscard]]
    static constexpr ValueType unsigned_multiply_without_affecting_flags(
        const ValueType a, const ValueType b, ValueType &high) noexcept
    {
        // Software fallback for MULX (BMI2)
        if constexpr(Width == OperandBitWidth::_32)
        {
            std::uint64_t res = static_cast<std::uint64_t>(a) * b;
            high              = static_cast<ValueType>(res >> 32);
            return static_cast<ValueType>(res);
        }
        else
        {
#if defined(__SIZEOF_INT128__)
            unsigned __int128 res = static_cast<unsigned __int128>(a) * b;
            high                  = static_cast<ValueType>(res >> 64);
            return static_cast<ValueType>(res);
#else
            // Fallback for missing int128 (e.g., 32-bit MSVC)
            std::uint32_t a_lo = static_cast<std::uint32_t>(a);
            std::uint32_t a_hi = static_cast<std::uint32_t>(a >> 32);
            std::uint32_t b_lo = static_cast<std::uint32_t>(b);
            std::uint32_t b_hi = static_cast<std::uint32_t>(b >> 32);
            std::uint64_t p0   = static_cast<std::uint64_t>(a_lo) * b_lo;
            std::uint64_t p1   = static_cast<std::uint64_t>(a_lo) * b_hi;
            std::uint64_t p2   = static_cast<std::uint64_t>(a_hi) * b_lo;
            std::uint64_t p3   = static_cast<std::uint64_t>(a_hi) * b_hi;
            std::uint32_t c32  = static_cast<std::uint32_t>(p0 >> 32);
            p1 += c32;
            p1 += p2;
            if(p1 < p2) p3 += 1ull << 32;
            high = p3 + (p1 >> 32);
            return (p1 << 32) | static_cast<std::uint32_t>(p0);
#endif
        }
    }
};

/**
 * Shio:
 * ABM (Advanced Bit Manipulation) is an x86 extension that adds fast
 * single-instruction implementations of common bit-counting operations.
 *
 * These operations are used heavily in algorithms that scan bitmasks or
 * maintain sparse sets (e.g. occupancy bitmaps, tiered allocators, and
 * component masks).
 *
 * We default to a portable software fallback, and override it if the
 * target supports ABM so we can emit a single-cycle POPCNT/LZCNT.
 */
template <OperandBitWidth Width>
struct ABM : FallbackBitManipulationInstructions<Width>
{
};

#if defined(__ABM__) || defined(__POPCNT__) || defined(__LZCNT__)
template <>
struct ABM<OperandBitWidth::_32>
    : FallbackBitManipulationInstructions<OperandBitWidth::_32>
{
    [[nodiscard]]
    static ValueType popcount(const ValueType mask) noexcept
    {
        return __popcntd(mask);
    }

    [[nodiscard]]
    static ValueType count_leading_zeros(const ValueType mask) noexcept
    {
        return _lzcnt_u32(mask);
    }
};

template <>
struct ABM<OperandBitWidth::_64>
    : FallbackBitManipulationInstructions<OperandBitWidth::_64>
{
    [[nodiscard]]
    static ValueType popcount(const ValueType mask) noexcept
    {
        return __popcntq(mask);
    }

    [[nodiscard]]
    static ValueType count_leading_zeros(const ValueType mask) noexcept
    {
        return _lzcnt_u64(mask);
    }
};
#endif // __ABM__

/**
 * Shio:
 * BMI1 is an x86 instruction set extension that provides efficient
 * bit-manipulation primitives (TZCNT, BLSR, BEXTR) for scanning and
 * extracting subranges from bitmasks.
 *
 * These operations are used in low-level allocators and entity/component
 * masks where finding the next set bit or clearing the low bit is frequent.
 *
 * When BMI1 is available, we override the software fallbacks to emit native
 * instructions and avoid branches or loops.
 */
template <OperandBitWidth Width>
struct BMI1 : ABM<Width> // Inherit ABM to chain features if available
{
};

#if defined(__BMI__)
template <>
struct BMI1<OperandBitWidth::_32> : ABM<OperandBitWidth::_32>
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

    [[nodiscard]]
    static ValueType bit_field_extract(
        const ValueType src, const ValueType control) noexcept
    {
        return _bextr_u32(src, control & 0xFF, (control >> 8) & 0xFF);
    }
};

template <>
struct BMI1<OperandBitWidth::_64> : ABM<OperandBitWidth::_64>
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

    [[nodiscard]]
    static ValueType bit_field_extract(
        const ValueType src, const ValueType control) noexcept
    {
        return _bextr_u64(src, control & 0xFF, (control >> 8) & 0xFF);
    }
};
#endif // __BMI__

/**
 * Shio:
 * BMI2 extends BMI1 with parallel bit extract/deposit and other instructions
 * that accelerate bitfield packing/unpacking and 128-bit multiply.
 *
 * These are critical for high-performance compact data structures and
 * for avoiding unpredictable loop-based bit fiddling.
 *
 * We only use the native instructions when the target supports BMI2.
 */
template <OperandBitWidth Width>
struct BMI2 : BMI1<Width>
{
};

#if defined(__BMI2__)
template <>
struct BMI2<OperandBitWidth::_32> : BMI1<OperandBitWidth::_32>
{
    [[nodiscard]]
    static ValueType parallel_bits_extract(
        const ValueType src, const ValueType mask) noexcept
    {
        return _pext_u32(src, mask);
    }

    [[nodiscard]]
    static ValueType parallel_bits_deposit(
        const ValueType src, const ValueType mask) noexcept
    {
        return _pdep_u32(src, mask);
    }

    [[nodiscard]]
    static ValueType bit_zone_clear(
        const ValueType src, const ValueType index) noexcept
    {
        return _bzhi_u32(src, index);
    }

    [[nodiscard]]
    static ValueType unsigned_multiply_without_affecting_flags(
        const ValueType a, const ValueType b, ValueType &high) noexcept
    {
        unsigned int h;
        ValueType    low = _mulx_u32(a, b, &h);
        high             = h;
        return low;
    }
};

template <>
struct BMI2<OperandBitWidth::_64> : BMI1<OperandBitWidth::_64>
{
    [[nodiscard]]
    static ValueType parallel_bits_extract(
        const ValueType src, const ValueType mask) noexcept
    {
        return _pext_u64(src, mask);
    }

    [[nodiscard]]
    static ValueType parallel_bits_deposit(
        const ValueType src, const ValueType mask) noexcept
    {
        return _pdep_u64(src, mask);
    }

    [[nodiscard]]
    static ValueType bit_zone_clear(
        const ValueType src, const ValueType index) noexcept
    {
        return _bzhi_u64(src, index);
    }

    [[nodiscard]]
    static ValueType unsigned_multiply_without_affecting_flags(
        const ValueType a, const ValueType b, ValueType &high) noexcept
    {
        unsigned long long h;
        ValueType          low = _mulx_u64(a, b, &h);
        high                   = h;
        return low;
    }
};
#endif // __BMI2__

/**
 * Shio:
 * Resolves the optimal default bit manipulation strategy for the target compile
 * architecture using PlatformTraits.
 * todo: better mixins
 */
template <OperandBitWidth Width>
using DefaultBitManipulationInstructions = std::conditional_t<
    PlatformTraits::has_bmi2(), BMI2<Width>,
    std::conditional_t<
        PlatformTraits::has_bmi1(), BMI1<Width>,
        FallbackBitManipulationInstructions<Width>
    >
>;

static_assert(BitManipulationInstructions<
    DefaultBitManipulationInstructions<OperandBitWidth::_32>
>);
static_assert(BitManipulationInstructions<
    DefaultBitManipulationInstructions<OperandBitWidth::_64>
>);
} // namespace usagi::platforms::instructions
