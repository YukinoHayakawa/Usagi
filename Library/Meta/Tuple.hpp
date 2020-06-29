#pragma once

#include <tuple>

namespace usagi
{
// Credit for struct Index:
// https://stackoverflow.com/questions/18063451/get-index-of-a-tuple-elements-type
template <class T, class Tuple>
struct Index;

template <class T, class... Types>
struct Index<T, std::tuple<T, Types...>>
{
    static const std::size_t value = 0;
};

template <class T, class U, class... Types>
struct Index<T, std::tuple<U, Types...>>
{
    static const std::size_t value = 1 + Index<T, std::tuple<Types...>>::value;
};

template <class T, class Tuple>
constexpr std::size_t INDEX = Index<T, Tuple>::value;

// Credit of has_type:
// https://stackoverflow.com/a/41171291/2150446

template <typename T, typename Tuple>
struct has_type;

template <typename T, typename... Us>
struct has_type<T, std::tuple<Us...>> : std::disjunction<std::is_same<T, Us>...>
{
};

template <typename T, typename Tuple>
constexpr bool has_type_v = has_type<T, Tuple>::value;

static_assert(has_type_v<int, std::tuple<int>>);
static_assert(!has_type_v<float, std::tuple<int>>);
static_assert(has_type_v<float, std::tuple<int, float>>);
}
