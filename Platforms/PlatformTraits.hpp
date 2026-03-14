#pragma once

// for hardware_destructive_interference_size
#include <new>

#include <Usagi/Library/Enums/Bitwise.hpp>

#if defined(__APPLE__)
    #include <TargetConditionals.h>
#endif

namespace usagi
{
namespace platforms
{
enum class Compiler : std::uint8_t
{
    Unknown    = 0,
    Clang      = 1 << 0,
    GCC        = 1 << 1,
    MSVC       = 1 << 2,
    AppleClang = (1 << 3) | Clang,
};

enum class Win32Versions : std::uint16_t
{
    WindowsXP  = 0x0501,
    Windows7   = 0x0601,
    Windows8   = 0x0602,
    Windows8_1 = 0x0603,
    Windows10  = 0x0A00,
    Windows11  = 0x0A00, // Win11 shares the same NT version as Win10
};
} // namespace platforms

template <>
struct EnableBitMaskOperators<platforms::Compiler> : std::true_type
{
};

namespace platforms
{
enum class Architecture : std::uint16_t
{
    Unknown        = 0,
    Bit32          = 1 << 0,
    Bit64          = 1 << 1,
    x86            = 1 << 2,
    x86_64         = x86 | Bit64,
    x86_32         = x86 | Bit32,
    ARM            = 1 << 3,
    ARM32          = ARM | Bit32,
    ARM64          = ARM | Bit64,
    WebAssembly    = 1 << 4,
    WebAssembly_32 = WebAssembly | Bit32,
    WebAssembly_64 = WebAssembly | Bit64,
};
} // namespace platforms

template <>
struct EnableBitMaskOperators<platforms::Architecture> : std::true_type
{
};

namespace platforms
{
enum class OperatingSystem : std::uint16_t
{
    Unknown     = 0,
    Windows     = 1 << 0,
    Linux       = 1 << 1,
    Darwin      = 1 << 2,
    MacOS       = 1 << 3 | Darwin,
    iOS         = 1 << 4 | Darwin,
    FreeBSD     = 1 << 5,
    PlayStation = 1 << 6 | FreeBSD,
    Xbox        = 1 << 7 | Windows,
    Android     = 1 << 8 | Linux,
    Switch      = 1 << 9,
};
} // namespace platforms

template <>
struct EnableBitMaskOperators<platforms::OperatingSystem> : std::true_type
{
};

namespace platforms
{
/**
 * Shio:
 * Const-evaluated platform traits resolved at compile-time.
 * Usagi Engine is fundamentally compiled with Clang, but we expose other
 * values for conceptual completeness and potential third-party library
 * integrations.
 */
struct PlatformTraits
{
    [[nodiscard]]
    static consteval Compiler compiler() noexcept
    {
        Compiler c = Compiler::Unknown;
#if defined(__clang__)
        c |= Compiler::Clang;
    #if defined(__apple_build_version__)
        c |= Compiler::AppleClang;
    #endif
#elif defined(__GNUC__)
        c |= Compiler::GCC;
#endif

// Note: A compiler can be both Clang and MSVC compatible (clang-cl)
#if defined(_MSC_VER)
        c |= Compiler::MSVC;
#endif
        return c;
    }

    [[nodiscard]]
    static consteval Architecture architecture() noexcept
    {
        Architecture a = Architecture::Unknown;
#if defined(__x86_64__) || defined(_M_X64)
        a |= Architecture::x86_64;
#elif defined(__i386__) || defined(_M_IX86)
        a |= Architecture::x86_32;
#elif defined(__aarch64__) || defined(_M_ARM64)
        a |= Architecture::ARM64;
#elif defined(__arm__) || defined(_M_ARM)
        a |= Architecture::ARM32;
#elif defined(__wasm64__)
        a |= Architecture::WebAssembly_64;
#elif defined(__wasm32__) || defined(__wasm__)
        a |= Architecture::WebAssembly_32;
#endif
        return a;
    }

    [[nodiscard]]
    static consteval OperatingSystem os() noexcept
    {
        OperatingSystem o = OperatingSystem::Unknown;
#if defined(_WIN32)
        o |= OperatingSystem::Windows;
    #if defined(_GAMING_XBOX) || defined(_XBOX_ONE)
        o |= OperatingSystem::Xbox;
    #endif
#elif defined(__APPLE__)
        o |= OperatingSystem::Darwin;
    #if TARGET_OS_IPHONE
        o |= OperatingSystem::iOS;
    #else
        o |= OperatingSystem::MacOS;
    #endif
#elif defined(__ANDROID__)
        o |= OperatingSystem::Android;
#elif defined(__linux__)
        o |= OperatingSystem::Linux;
#elif defined(__FreeBSD__) || defined(__ORBIS__) || defined(__PROSPERO__)
        o |= OperatingSystem::FreeBSD;
    #if defined(__ORBIS__) || defined(__PROSPERO__)
        o |= OperatingSystem::PlayStation;
    #endif
#elif defined(__NN__)
        o |= OperatingSystem::Switch;
#endif
        return o;
    }

    /**
     * @brief Evaluates the compilation target's OS version requirement using
     * predefined macros. Returns 0x0A00 (Win10) by default for Windows if
     * unspecified.
     */
    [[nodiscard]]
    static consteval std::uint32_t target_os_version() noexcept
    {
#if defined(_WIN32)
    #if defined(_WIN32_WINNT)
        return _WIN32_WINNT;
    #else
        return std::to_underlying(Win32Versions::Windows10);
    #endif
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
        return __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__;
#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
        return __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__;
#elif defined(__ANDROID_API__)
        return __ANDROID_API__;
#else
        return 0; // Unknown or not explicitly targeted
#endif
    }

    [[nodiscard]]
    static consteval bool is_static_analysis() noexcept
    {
#if defined(__RESHARPER__) || defined(__INTELLISENSE__) || \
    defined(__clang_analyzer__)
        return true;
#else
        return false;
#endif
    }

    [[nodiscard]]
    static consteval bool is_clang_format() noexcept
    {
        // clang-format does not run the preprocessor in a way that allows us
        // to check a macro. We'll return false here unless specifically
        // configured via a custom macro if needed later, but standard
        // macros like __RESHARPER__ are more robust for static analysis.
#if defined(USAGI_IS_CLANG_FORMAT)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Bit Manipulation Instruction Set 1
     * Provides TZCNT (Trailing Zeros Count), ANDN (Logical And Not), BLSR
     * (Reset lowest set bit), etc.
     */
    [[nodiscard]]
    static consteval bool has_bmi1() noexcept
    {
#if defined(__BMI__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Bit Manipulation Instruction Set 2
     * Provides PDEP (Parallel bits deposit), PEXT (Parallel bits extract), BZHI
     * (Zero high bits), etc.
     */
    [[nodiscard]]
    static consteval bool has_bmi2() noexcept
    {
#if defined(__BMI2__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Advanced Bit Manipulation (AMD / Intel)
     * Provides POPCNT (Population count) and LZCNT (Leading Zeros Count).
     */
    [[nodiscard]]
    static consteval bool has_abm() noexcept
    {
#if defined(__ABM__) || defined(__LZCNT__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Trailing Bit Manipulation (AMD specific)
     * Provides instructions like BLCS (Set trailing zero bits), BLCFILL (Fill
     * trailing zeros), etc.
     */
    [[nodiscard]]
    static consteval bool has_tbm() noexcept
    {
#if defined(__TBM__)
        return true;
#else
        return false;
#endif
    }

    // --- x86 / x86_64 SIMD & Vector Extensions ---

    /**
     * @brief Streaming SIMD Extensions
     * Introduces 128-bit XMM registers. Operates on 4 single-precision floats
     * simultaneously.
     */
    [[nodiscard]]
    static consteval bool has_sse() noexcept
    {
#if defined(__SSE__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1) || \
    defined(_M_X64)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Streaming SIMD Extensions 2
     * Extends SSE to support double-precision floats (2x64-bit) and
     * 8/16/32/64-bit integers in XMM.
     */
    [[nodiscard]]
    static consteval bool has_sse2() noexcept
    {
#if defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2) || \
    defined(_M_X64)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Streaming SIMD Extensions 3
     * Adds horizontal/asymmetric operations (e.g. ADDSUB, HADD) useful for
     * complex arithmetic/DSP.
     */
    [[nodiscard]]
    static consteval bool has_sse3() noexcept
    {
#if defined(__SSE3__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Supplemental Streaming SIMD Extensions 3
     * Adds powerful byte-shuffle instructions (PSHUFB) and absolute value
     * instructions.
     */
    [[nodiscard]]
    static consteval bool has_ssse3() noexcept
    {
#if defined(__SSSE3__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Streaming SIMD Extensions 4.1
     * Adds dot product instructions, min/max for floats, and flexible blend
     * instructions.
     */
    [[nodiscard]]
    static consteval bool has_sse4_1() noexcept
    {
#if defined(__SSE4_1__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Streaming SIMD Extensions 4.2
     * Adds powerful string/text processing instructions (PCMPESTRI) and CRC32
     * hardware acceleration.
     */
    [[nodiscard]]
    static consteval bool has_sse4_2() noexcept
    {
#if defined(__SSE4_2__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Advanced Vector Extensions
     * Widens registers to 256-bits (YMM). Supports 8 single-precision floats.
     * Introduces 3-operand syntax (dest = src1 + src2).
     */
    [[nodiscard]]
    static consteval bool has_avx() noexcept
    {
#if defined(__AVX__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Advanced Vector Extensions 2
     * Extends AVX 256-bit support to integer operations. Adds gather/scatter,
     * cross-lane shuffles (PERMD).
     */
    [[nodiscard]]
    static consteval bool has_avx2() noexcept
    {
#if defined(__AVX2__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Advanced Vector Extensions 512 (Foundation)
     * Widens registers to 512-bits (ZMM). 16 single-precision floats. Adds
     * opmask registers for predication.
     */
    [[nodiscard]]
    static consteval bool has_avx512f() noexcept
    {
#if defined(__AVX512F__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Fused Multiply-Add
     * Calculates (A * B) + C with infinite precision before rounding, improving
     * speed and accuracy.
     */
    [[nodiscard]]
    static consteval bool has_fma() noexcept
    {
#if defined(__FMA__)
        return true;
#else
        return false;
#endif
    }

    // --- ARM Extensions ---

    /**
     * @brief Advanced SIMD (NEON)
     * ARM's 128-bit vector architecture for DSP, graphics, and machine
     * learning.
     */
    [[nodiscard]]
    static consteval bool has_neon() noexcept
    {
#if defined(__ARM_NEON)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Scalable Vector Extension
     * ARM's vector extension allowing vector lengths from 128 to 2048 bits
     * without recompiling code.
     */
    [[nodiscard]]
    static consteval bool has_sve() noexcept
    {
#if defined(__ARM_FEATURE_SVE)
        return true;
#else
        return false;
#endif
    }

    // --- WebAssembly Extensions ---

    /**
     * @brief WebAssembly 128-bit SIMD
     * Introduces 128-bit vector operations (v128) natively to WebAssembly
     * environments.
     */
    [[nodiscard]]
    static consteval bool has_wasm_simd128() noexcept
    {
#if defined(__wasm_simd128__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Returns the minimum offset between two objects to avoid false
     * sharing. Often reflects the architecture's typical L1 cache line size
     * (e.g., 64 bytes for x86_64).
     */
    [[nodiscard]]
    static consteval std::size_t cpu_cache_line_size() noexcept
    {
        return std::hardware_destructive_interference_size;
    }

    /**
     * @brief Checks if the compiler supports 128-bit integers (__int128 /
     * unsigned __int128).
     */
    [[nodiscard]]
    static consteval bool has_int128() noexcept
    {
#if defined(__SIZEOF_INT128__)
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Checks if the compiler supports long double.
     */
    [[nodiscard]]
    static consteval bool has_long_double() noexcept
    {
#if defined(__SIZEOF_LONG_DOUBLE__)
        return true;
#else
        return false; // Though strictly speaking, standard C++ guarantees long
                      // double exists.
#endif
    }
};

namespace details::static_tests
{
// Shio: Static checks verifying cache line size constants on known
// architectures.
consteval bool check_cache_line_size()
{
    if constexpr(has_any_of(
                     PlatformTraits::architecture(), Architecture::Bit64))
    {
        // 64 bytes is the standard L1 cache line size for typical x86_64 and
        // ARM64 CPUs. Some specialized server chips (like Intel Xeon/PowerPC)
        // might use 128 bytes, but for standard Usagi Engine deployment
        // profiles, 64 is the baseline standard.
        return PlatformTraits::cpu_cache_line_size() >= 64;
    }
    else
    {
        return true;
    }
}

static_assert(check_cache_line_size(),
    "Cache line size unexpectedly small for target architecture.");
} // namespace details::static_tests

/**
 * Shio:
 * A combinable bitmask aggregating all compile-time evaluated instruction sets.
 * This is incredibly useful for runtime capability reporting or generic
 * programming guards.
 */
enum class AvailableInstructionSets : std::uint32_t
{
    None = 0,

    // Bit Manipulation
    ABM  = PlatformTraits::has_abm() << 0,
    BMI1 = PlatformTraits::has_bmi1() << 1,
    BMI2 = PlatformTraits::has_bmi2() << 2,
    TBM  = PlatformTraits::has_tbm() << 3,

    // x86 SIMD
    SSE     = PlatformTraits::has_sse() << 4,
    SSE2    = PlatformTraits::has_sse2() << 5,
    SSE3    = PlatformTraits::has_sse3() << 6,
    SSSE3   = PlatformTraits::has_ssse3() << 7,
    SSE4_1  = PlatformTraits::has_sse4_1() << 8,
    SSE4_2  = PlatformTraits::has_sse4_2() << 9,
    AVX     = PlatformTraits::has_avx() << 10,
    AVX2    = PlatformTraits::has_avx2() << 11,
    AVX512F = PlatformTraits::has_avx512f() << 12,
    FMA     = PlatformTraits::has_fma() << 13,

    // ARM SIMD
    NEON = PlatformTraits::has_neon() << 14,
    SVE  = PlatformTraits::has_sve() << 15,

    // WASM SIMD
    WebAsmSIMD128 = PlatformTraits::has_wasm_simd128() << 16,
};
} // namespace platforms

template <>
struct EnableBitMaskOperators<platforms::AvailableInstructionSets>
    : std::true_type
{
};

namespace platforms
{
/**
 * @brief Aggregates all detected instruction sets into a single bitmask.
 */
[[nodiscard]]
consteval AvailableInstructionSets available_instruction_sets() noexcept
{
    auto s = AvailableInstructionSets::None;

    s |= AvailableInstructionSets::ABM;
    s |= AvailableInstructionSets::BMI1;
    s |= AvailableInstructionSets::BMI2;
    s |= AvailableInstructionSets::TBM;

    s |= AvailableInstructionSets::SSE;
    s |= AvailableInstructionSets::SSE2;
    s |= AvailableInstructionSets::SSE3;
    s |= AvailableInstructionSets::SSSE3;
    s |= AvailableInstructionSets::SSE4_1;
    s |= AvailableInstructionSets::SSE4_2;
    s |= AvailableInstructionSets::AVX;
    s |= AvailableInstructionSets::AVX2;
    s |= AvailableInstructionSets::AVX512F;
    s |= AvailableInstructionSets::FMA;

    s |= AvailableInstructionSets::NEON;
    s |= AvailableInstructionSets::SVE;

    s |= AvailableInstructionSets::WebAsmSIMD128;

    return s;
}
} // namespace platforms
} // namespace usagi
