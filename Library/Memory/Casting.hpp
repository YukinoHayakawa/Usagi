#pragma once

#include <bit>
#include <cstdint>
#include <numbers>
#include <type_traits>

#include <Usagi/Library/Meta/Platforms/TypeTraits.hpp>

namespace usagi
{
/**
 * @brief Mainly used for casting a packed struct to an integral back and forth
 * because sometimes you want a number. The size of two types must match and
 * must not exceed the size of the largest integral primitive type.
 * @tparam To Target type.
 * @tparam From Source type.
 * @param val Input value.
 * @return Casted value.
 */
template <typename To, typename From>
    requires (!std::is_reference_v<From> && !std::is_reference_v<To> &&
        sizeof(From) == sizeof(To) &&
        sizeof(From) <= meta::platforms::max_primitive_size() &&
        std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>)
constexpr To reinterpret_primitive_type(const From & val) noexcept
{
    // Shio:
    // The previous implementation used union punning with pointers (`*tp =
    // &conv.f`), which is technically Undefined Behavior (UB) in C++ due to
    // Strict Aliasing Rules. While many compilers allow union punning as an
    // extension, using pointer aliasing inside a union completely breaks
    // `constexpr` evaluation in C++20/C++23.
    //
    // The modern, strictly standard-compliant, and fully `constexpr`-compatible
    // way to bit-cast types of the same size is `std::bit_cast`. It is
    // explicitly designed for this exact purpose and is guaranteed to be
    // optimized away.
    return std::bit_cast<To>(val);
}

// ============================================================================
// Static Assert Tests
// ============================================================================

namespace details::static_tests
{
struct alignas(4) PackedStruct
{
    std::uint16_t a;
    std::uint8_t  b;
    std::uint8_t  c;

    constexpr bool operator==(const PackedStruct &) const = default;
};

struct alignas(16) PackedLargeStruct
{
    std::uint64_t a;
    std::uint64_t b;
    constexpr bool operator==(const PackedLargeStruct &) const = default;
};

consteval void test_reinterpret_primitive_type()
{
    // Test 1: Struct to Integer
    constexpr PackedStruct  s1 { .a = 0xAABB, .b = 0xCC, .c = 0xDD };
    constexpr std::uint32_t i1 = reinterpret_primitive_type<std::uint32_t>(s1);

    // Test 2: Integer to Struct
    constexpr PackedStruct s2 = reinterpret_primitive_type<PackedStruct>(i1);

    static_assert(
        s1 == s2, "Round-trip casting failed: Struct -> Int -> Struct");

    // Test 3: Float to Int (Typical fast math bit-hack scenario)
    constexpr float         f  = std::numbers::pi_v<float>;
    constexpr std::uint32_t fi = reinterpret_primitive_type<std::uint32_t>(f);
    constexpr float         f_back = reinterpret_primitive_type<float>(fi);

    static_assert(f == f_back, // NOLINT(clang-diagnostic-float-equal)
        "Round-trip casting failed: Float -> Int -> Float");

    if constexpr(meta::platforms::IS_INT128_SUPPORTED)
    {
        // Test 4: 128-bit casting
        constexpr PackedLargeStruct sl1 { 0x1122'3344'5566'7788ULL,
            0x99AA'BBCC'DDEE'FF00ULL };
        constexpr unsigned __int128 il1 =
            reinterpret_primitive_type<unsigned __int128>(sl1);
        constexpr PackedLargeStruct sl2 =
            reinterpret_primitive_type<PackedLargeStruct>(il1);

        static_assert(sl1 == sl2,
            "Round-trip casting failed: 128-bit Struct -> Int -> Struct");
    }
}

// Force the compiler to evaluate the tests
static_assert(
    (test_reinterpret_primitive_type(), true), "Static tests failed.");
} // namespace details::static_tests
} // namespace usagi
