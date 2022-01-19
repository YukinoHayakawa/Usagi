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
    static constexpr std::size_t value = 0;
};

template <class T, class U, class... Types>
struct Index<T, std::tuple<U, Types...>>
{
    static constexpr std::size_t value =
        1 + Index<T, std::tuple<Types...>>::value;
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

template <typename T, typename Tuple>
struct ConstructibleFromTupleHelper
{
    constexpr static std::size_t N = std::tuple_size_v<Tuple>;
    using IndexSeq = std::make_index_sequence<N>;

    template <std::size_t... I>
    static constexpr bool do_test(std::index_sequence<I...>)
    {
        return std::is_constructible_v<T, std::tuple_element_t<I, Tuple>...>;
    }

    constexpr static bool value = do_test(IndexSeq());
};

template <typename T, typename Tuple>
concept ConstructibleFromTuple = ConstructibleFromTupleHelper<T, Tuple>::value;

template <typename Tuple>
struct NoRvalueRefInTupleHelper;

template <typename... Args>
struct NoRvalueRefInTupleHelper<std::tuple<Args...>>
{
    constexpr static bool value()
    requires (... && (!std::is_rvalue_reference_v<Args>))
    {
        return true;
    }
};

template <typename Tuple>
concept NoRvalueRefInTuple = NoRvalueRefInTupleHelper<Tuple>::value();
}
