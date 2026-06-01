#pragma once

namespace usagi::compile_env
{
/*
 * freestanding means the compilation unit will assume no host OS environment.
 * this is required for compiling game scripts.
 * note that without a host env, RTTI and exceptions also have to be disabled
 * cuz related syscalls cannot be made.
 *
 * Affected things with `-ffreestanding`:
 * - no `main()` or other OS-specific functions assumed as entry point.
 * - no implicit library linking -> no hidden OS calls.
 * - no OS-specific optimization.
 */
consteval bool is_freestanding()
{
// standard macro. is 1 for hosted, 0 for freestanding.
#if defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0)
    return true;
#else
    return false;
#endif
}

/*
 * Affected things with `-fno-rtti`:
 * - `type_info` gone. no `dynamic_cast` or `typeid`.
 * - vtables gonna be thinner cuz no refs to `type_info`.
 * - no down-casting from a virtual base to derived cuz no `dynamic_cast`.
 * - can't use `std::any` cuz no `typeid`.
 * - no `std::dynamic_pointer_cast`.
 * - `std::get_deleter` would fail cuz `shared_ptr` type-erases the deleter.
 * - can only `catch(...)` cuz no `typeid` no type matching.
 */
consteval bool has_rtti()
{
// standard || msvc || gcc/clang
#if defined(__cpp_rtti) || defined(_CPPRTTI) || defined(__GXX_RTTI)
    return true;
#else
    return false;
#endif
}

/*
 * Affected things with `-fno-exceptions`:
 * - using `try/catch/throw` -> compile errors.
 * - no unwind tables (`.eh_frame` sections).
 * - no runtime unwinder (no more syscalls made).
 * - functions might be thinner as well cuz no more code for unwinding.
 */
consteval bool has_exceptions()
{
// standard || msvc || gcc/clang
#if defined(__cpp_exceptions) || defined(_CPPUNWIND) || defined(__EXCEPTIONS)
    return true;
#else
    return false;
#endif
}

/*
 * In a freestanding environment (up to C++26), the following headers are
 * available:
 *
 * Language Support & Fundamental Types
 * - `<cfloat>`           // Floating-point limits
 * - `<climits>`          // Integer limits
 * - `<cstdarg>`          // Variadic arguments
 * - `<cstddef>`          // Standard types (size_t, nullptr_t)
 * - `<cstdint>`          // Fixed-width integers
 * - `<cstdlib>`          // Partial: abort, atexit, exit, etc.
 * - `<exception>`        // Exception handling base (std::exception,
 * std::terminate)
 * - `<initializer_list>` // Initializer lists
 * - `<limits>`           // std::numeric_limits
 * - `<new>`              // Low-level memory management (placement new,
 * launcher)
 * - `<source_location>`  // Capture file/line info (C++20)
 * - `<stdfloat>`         // Extended floating-point types (C++23)
 * - `<typeinfo>`         // RTTI base (required, even if -fno-rtti weakens it)
 * - `<version>`          // Feature test macros
 *
 * Concepts & Metaprogramming (Compile-time only)
 * - `<compare>`          // Three-way comparison (<=>)
 * - `<concepts>`         // Standard concepts (C++20)
 * - `<ratio>`            // Compile-time rational arithmetic
 * - `<type_traits>`      // Type transformations and checks
 *
 * General Utilities
 * - `<bit>`              // Bit manipulation (C++20)
 * - `<bitset>`           // Fixed-size bit array (Freestanding in C++23)
 * - `<charconv>`         // Low-level integer/float conversion (from_chars,
 * to_chars)
 * - `<debugging>`        // Debugging support (std::breakpoint) (C++26)
 * - `<expected>`         // Error handling wrapper (C++23)
 * - `<functional>`       // Partial: std::invoke, std::reference_wrapper, etc.
 * - `<optional>`         // Optional values (C++23)
 * - `<tuple>`            // Tuples (C++23)
 * - `<utility>`          // std::move, std::forward, std::pair
 * - `<variant>`          // Type-safe unions (C++23)
 *
 * Memory & Pointers
 * - `<memory>`           // Partial: std::unique_ptr, std::addressof,
 * std::construct_at
 *
 * Containers & Views (Non-allocating)
 * - `<array>`            // std::array
 * - `<inplace_vector>`   // Dynamically resizable vector on the stack (C++26)
 * - `<mdspan>`           // Multi-dimensional array views (C++23)
 * - `<span>`             // Contiguous memory views (C++20)
 * - `<string_view>`      // String views
 *
 * Algorithms, Iterators & Math
 * - `<algorithm>`        // Partial: Non-allocating algorithms
 * - `<iterator>`         // Iterator tags and primitives
 * - `<numeric>`          // Partial: std::accumulate, std::reduce, etc.
 * - `<numbers>`          // Math constants (pi, e, phi) (C++20)
 * - `<random>`           // Partial: Generators/Engines (Mersenne Twister), no
 * random_device
 * - `<ranges>`           // Range adaptors and views (C++20)
 *
 * Concurrency & System
 * - `<atomic>`           // Partial: atomic_flag, lock-free atomics (no
 * waiting/notify)
 * - `<coroutine>`        // Coroutine support traits (C++20)
 *
 * C-Library Wrappers
 * - `<cassert>`          // assert macro
 * - `<cstring>`          // memcpy, memset, etc.
 */
consteval bool is_freestanding_ready()
{
    return is_freestanding() && !has_rtti() && !has_exceptions();
}
} // namespace usagi::compile_env
