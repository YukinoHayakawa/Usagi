#pragma once

#include <type_traits>
#include <cstddef>

namespace usagi
{
// Code from
// https://stackoverflow.com/questions/26169198/how-to-get-the-index-of-a-type-in-a-variadic-type-pack

// Base template
template <typename T, typename... Ts>
struct ParameterPackIndex;

// T is the same as the first in the parameter pack, return 0.
template <typename T, typename... Ts>
struct ParameterPackIndex<T, T, Ts...>
    : std::integral_constant<
        std::size_t,
        0
    >
{};

// Otherwise, continue linear search.
template <typename T, typename U, typename... Ts>
struct ParameterPackIndex<T, U, Ts...>
    : std::integral_constant<
        std::size_t,
        1 + ParameterPackIndex<T, Ts...>::value
    >
{};

template <typename T, typename... Ts>
constexpr std::size_t ParameterPackIndex_v =
    ParameterPackIndex<T, Ts...>::value;
}
