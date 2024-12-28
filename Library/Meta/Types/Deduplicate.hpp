#pragma once

#include <type_traits>
#include <utility>
#include <bitset>
#include <algorithm>

#include <Usagi/Library/Meta/Types/Containers/List/TypeList.hpp>
#include <Usagi/Library/Meta/Types/Containers/List/Concatenate.hpp>

#include "Functional.hpp"

namespace usagi::meta::types
{
namespace details
{
/*
 * Computes whether the current type has already appeared in the previous
 * part of the type list.
 * T is the current type. PrecedingIndices... should be [0, index), where
 * index is the index of T in Ts...
 * So basically this function checks whether T is present in
 * Ts...[PrecedingIndices]..., which are the type before T.
 * std::make_index_sequence<N>() gives std::integer_sequence<
 * [...], { 0, ..., N-1 }>. Therefore, it automatically excludes the current
 * type being tested. i.e. When N == 0, it gives <[...], {}>, automatically
 * marks the current type as unique.
 * I hope the compiler does some internal caching. Otherwise, the algorithm
 * might be O(num_types^2).
 */
template <typename T, typename... Ts, typename Int, Int... PrecedingIndices>
consteval bool is_type_duplicated(
    std::integer_sequence<Int, PrecedingIndices...>)
{
    return std::disjunction_v<std::is_same<T, Ts...[PrecedingIndices]>...>;
}

/*
 * Static Tests
 */

// unique type.
static_assert(is_type_duplicated<int, int>(
    std::make_index_sequence<0>()) == false);
// the second type is not reached.
static_assert(is_type_duplicated<bool, int, bool>(
    std::make_index_sequence<0>()) == false);
// still, only checked against int.
static_assert(is_type_duplicated<bool, int, bool, bool>(
    std::make_index_sequence<1>()) == false);
// duplicate found on index 1. 
static_assert(is_type_duplicated<bool, int, bool, bool>(
    std::make_index_sequence<2>()) == true);
}

/**
 * Calculate the uniqueness of provided type list. The first appearance of a
 * type in the list is regarded as unique and the bit with the same index in
 * the returned string is set to '1'.
 * @tparam Ts List of types.
 * @return a std::string where each char corresponds the uniqueness of a type,
 * where '0' means the type is duplicated in the list, and '1' means it is
 * unique.
 */
template <typename... Ts>
consteval auto calc_unique_type_array()
{
    constexpr std::size_t num_types = sizeof...(Ts);
    std::bitset<num_types> is_type_unique;
    // check whether each type is duplicated starting from the first one.
    map<Ts...>([&]<std::size_t CurIndex, typename CurT> {
        is_type_unique[CurIndex] = !details::is_type_duplicated<CurT, Ts...>(
            // make_index_sequence automatically creates a
            // sequence ending with I - 1.
            std::make_index_sequence<CurIndex>()
        );
    });
    auto uniqueness_array = is_type_unique.to_string();
    std::ranges::reverse(uniqueness_array);
    return uniqueness_array;
}

/*
 * Static Tests.
 */
static_assert(calc_unique_type_array<int, int, double, char>() == "1011");

/**
 * Remove duplicated types from Ts... and wrap the result in List.
 * @tparam List A template accepting variadic types.
 * @tparam Ts A list of types.
 * @return List<Us...> where Us... are unique types from Ts...
 */
template <
    template <typename...> typename List,
    typename... Ts
>
consteval auto deduplicate_type_list()
{
    if constexpr(sizeof...(Ts) == 0)
    {
        return List<>();
    }
    else
    {
        // static constexpr auto unique_flags = calc_unique_type_array<Ts...>() + std::string("0");
        // auto test = [&]<typename> { 
        //     static_assert(unique_flags.size() == sizeof...(Ts));
        // };
        // test.template operator()<int>();
        // process types after U. remove front type one-by-one
        // tail_unique_flags does not include value for U
        // <U, Us...> == <Ts...> during the first call
        // I is the index of U in Ts...
        // constexpr std::size_t num_types = sizeof...(Ts);
        // note that unique_flags MUST be part of the template argument,
        // otherwise type deduction may result in inconsistent outcomes.
        auto op_reduce = [&]<
            std::size_t I,
            typename U,
            typename... Us,
            bool KeepCurT = (calc_unique_type_array<Ts...>() + std::string("0"))[I + 1] == '1'
        >(auto &&self)
            // requires (unique_flags.size() == sizeof...(Ts))
        {
            // return List<>();
            // static_assert(unique_flags.size() == sizeof...(Ts));
            // base case. nothing left.
            if constexpr(I + 1 >= sizeof...(Ts))
            {
                return List<>();
            }
            // else
            // {
                // return List<>();
            // }
            // decide whether to keep Us...[0]
            // keep Us...[0]
            if constexpr(KeepCurT)
            {
                return concatenate_lists(
                    List<Us...[0]>(),
                    self.template operator()<I + 1, Us...>(self)
                );
            }
            // drop Us...[0]
            else if constexpr(sizeof...(Us) > 0)
            {
                return self.template operator()<I + 1, Us...>(self);
            }
            else
            {
                return List<>();
            }
        };
        return concatenate_lists(
            List<Ts...[0]>(),
            op_reduce.template operator()<0, Ts...>(op_reduce)
        );
    }
}

static_assert(std::is_same_v<
    decltype(deduplicate_type_list<containers::TypeList>()),
    containers::TypeList<>
>);
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<containers::TypeList, int>()),
    containers::TypeList<int>
>);
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<containers::TypeList, int, int, bool>()),
    containers::TypeList<int, bool>
>);
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<containers::TypeList, int, char, int, bool>()),
    containers::TypeList<int, char, bool>
>);
}
