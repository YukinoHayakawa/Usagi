#pragma once

#include <type_traits>
#include <concepts>

namespace usagi::meta::concepts
{
/*
 * An invocable that accepts the index of a type with the type as template
 * parameters. The type of the index is supposed to be std::size_t, or at least
 * some unsigned integral type. If you force any sort of implicit conversion
 * by using other kinds of types, you are at your own risk.
 */
template <typename Func, typename T>
concept GenericIndexedTypenameOperator = requires(Func op)
{
    op.template operator()<
        // basically the same idea. but the second one is easier to write.
        // std::integral_constant<std::size_t, 0>::value,
        static_cast<std::size_t>(0),
        T
    >();
};

// expected usage
static_assert(GenericIndexedTypenameOperator<
    decltype([]<std::size_t, typename> { }),
    void
>);

// missing typename parameter
static_assert(!GenericIndexedTypenameOperator<
    decltype([]<std::size_t> { }),
    void
>);

// missing index parameter
static_assert(!GenericIndexedTypenameOperator<
    decltype([]<typename> { }),
    void
>);

// inverted parameters
static_assert(!GenericIndexedTypenameOperator<
    decltype([]<typename, std::size_t> { }),
    void
>);

// float does not implicitly convert to std::size_t
static_assert(!GenericIndexedTypenameOperator<
    decltype([]<float, typename> { }),
    void
>);

template <typename Func>
concept IndexedBooleanOperator = requires(Func op)
{
    { op.template operator()<static_cast<std::size_t>(0)>() }
        -> std::same_as<bool>;
};

static_assert(IndexedBooleanOperator<
    decltype([]<std::size_t> { return true; })
>);

static_assert(!IndexedBooleanOperator<
    decltype([]<std::size_t> { return 1; })
>);
}
