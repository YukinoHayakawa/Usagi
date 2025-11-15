#pragma once

#include <type_traits>

#include <Usagi/Library/Meta/Reflection/StaticReflection.hpp>

namespace usagi::meta
{
/*
 * Shio: Checks if a type T is a const-qualified value or reference, but not
 * a pointer. This is useful for distinguishing between `const int` and
 * `int * const`. This concept evaluates to true if T, after removing any
 * reference qualifiers, is const-qualified and is not a pointer type.
 *
 * Yukino: Checks whether a value or reference is with constness and thus not
 * mutable. This concept would reject any pointer types. For pointer types, use
 * `ImmutablePointer` and `PointerToImmutableValue`.
 */
template <typename T>
concept Immutable = !std::is_pointer_v<std::remove_reference_t<T>> &&
    std::is_const_v<std::remove_reference_t<T>>;

/*
 * Shio: Checks if a type T is a const-qualified pointer (e.g.,
 * `int * const`), meaning the pointer itself cannot be reseated. This
 * concept evaluates to true if T is a pointer type that is const-qualified.
 * It does not consider the constness of the pointed-to value.
 *
 * Yukino: Checks whether `T` itself as a pointer is immutable, regardless of
 * the values it points to.
 */
template <typename T>
concept ImmutablePointer = std::is_pointer_v<std::remove_reference_t<T>> &&
    std::is_const_v<std::remove_reference_t<T>>;

/*
 * Shio: Checks if a type T is a pointer to a const-qualified value (e.g.,
 * `const int *`), meaning the value pointed to cannot be modified through
 * this pointer. This concept evaluates to true if T is a pointer type, and
 * the type it points to is const-qualified. It does not consider the
 * constness of the pointer itself. Note that this checks only one level of
 * indirection. For `const int**`, it checks if `const int*` is const, which
 * is false.
 *
 * Yukino: Checks whether `T` points to immutable values, regardless of the
 * constness of `T`.
 */
template <typename T>
concept PointerToImmutableValue =
    std::is_pointer_v<std::remove_reference_t<T>> &&
    std::is_const_v<std::remove_pointer_t<std::remove_reference_t<T>>>;

/*
 * Shio: A type trait that aligns the const-qualification of `TypeToAlign` with
 * the constness of `ConstnessRef`.
 *
 * If `ConstnessRef` satisfies the `Immutable` concept (i.e., it's a const
 * value or reference, but not a pointer), this trait adds `const` to
 * `TypeToAlign`. Otherwise, it removes `const` from `TypeToAlign`.
 *
 * This is useful for generic programming where the constness of one type
 * needs to be propagated to another.
 */
template <typename ConstnessRef, typename TypeToAlign>
using align_constness_t = typename[:std::meta::dealias([] consteval {
    if constexpr(Immutable<ConstnessRef>)
        return std::meta::add_const(^^TypeToAlign);
    else
        return std::meta::remove_const(^^TypeToAlign);
}()):];

namespace static_tests
{
static_assert(Immutable<const int>);
static_assert(Immutable<const int &>);

// Constness binds to the **pointer**.
static_assert(!Immutable<int * const>);
static_assert(ImmutablePointer<int * const>);
static_assert(ImmutablePointer<int * const &>);
static_assert(!ImmutablePointer<int *>);
static_assert(!PointerToImmutableValue<int * const>);
static_assert(PointerToImmutableValue<const int * const>);

// Constness binds to the value pointed by the pointer.
static_assert(!Immutable<const int *>);
static_assert(!ImmutablePointer<const int *>);
static_assert(PointerToImmutableValue<const int *>);
static_assert(PointerToImmutableValue<const int *&>);
static_assert(!PointerToImmutableValue<int *>);
static_assert(PointerToImmutableValue<const int *>);
static_assert(PointerToImmutableValue<const int *&>);

static_assert(!Immutable<int>);
static_assert(!Immutable<int &>);
static_assert(!Immutable<int *>);

static_assert(std::is_same_v<align_constness_t<const int, float>, const float>);
static_assert(
    std::is_same_v<align_constness_t<const int &, float>, const float>
);
static_assert(
    std::is_same_v<align_constness_t<const int &&, float>, const float>
);
static_assert(
    std::is_same_v<
        std::remove_volatile_t<align_constness_t<const volatile int, float>>,
        const float
    >
);
static_assert(
    std::is_same_v<
        std::remove_volatile_t<align_constness_t<const volatile int &, float>>,
        const float
    >
);
static_assert(
    std::is_same_v<
        std::remove_volatile_t<align_constness_t<const volatile int &&, float>>,
        const float
    >
);
static_assert(std::is_same_v<align_constness_t<int, float>, float>);
static_assert(std::is_same_v<align_constness_t<int &, float>, float>);
static_assert(std::is_same_v<align_constness_t<int &&, float>, float>);
static_assert(std::is_same_v<align_constness_t<int *, float>, float>);
static_assert(std::is_same_v<align_constness_t<int *&, float>, float>);
static_assert(std::is_same_v<align_constness_t<int *&&, float>, float>);

// Shio: Additional proposed tests for edge cases and multi-level pointers.
static_assert(!Immutable<const int * const>);
static_assert(Immutable<const volatile int>);

static_assert(ImmutablePointer<const volatile int * const>);
static_assert(ImmutablePointer<int * const volatile>);

static_assert(PointerToImmutableValue<const int * volatile>);
static_assert(PointerToImmutableValue<const volatile int *>);
static_assert(PointerToImmutableValue<const volatile int * const>);

// Shio: Tests for multi-level pointers. The concepts only check the first
// level of indirection or qualification.
static_assert(!Immutable<int **>);
static_assert(!ImmutablePointer<int **>);
static_assert(!PointerToImmutableValue<int **>);

// Shio: `int * const *` is a pointer to a `int * const`. The pointed-to
// type `int * const` is const-qualified, so `PointerToImmutableValue` is true.
static_assert(PointerToImmutableValue<int * const *>);
static_assert(!ImmutablePointer<int * const *>);

// Shio: `int ** const` is a const pointer to a `int *`. The pointer itself
// is const, so `ImmutablePointer` is true.
static_assert(ImmutablePointer<int ** const>);
static_assert(!PointerToImmutableValue<int ** const>);

// Shio: `const int **` is a pointer to a `const int *`. The pointed-to
// type `const int *` is a pointer, not a const-qualified pointer, so
// `PointerToImmutableValue` is false. This demonstrates the one-level check.
static_assert(!PointerToImmutableValue<const int **>);

// Shio: Additional tests for align_constness_t with pointer types.
static_assert(std::is_same_v<align_constness_t<int * const, float>, float>);
static_assert(std::is_same_v<align_constness_t<const int *, float>, float>);
static_assert(
    std::is_same_v<align_constness_t<const int * const, float>, float>
);
} // namespace static_tests
} // namespace usagi::meta
