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

// Handling of empty pack.
static_assert(calc_unique_type_array<>().empty());
// Common cases.
static_assert(calc_unique_type_array<int>() == "1");
static_assert(calc_unique_type_array<int, int, double, char>() == "1011");

/**
 * Remove duplicated types from Ts... and wrap the result in List.
 * @tparam List A template accepting variadic types.
 * @tparam Ts A list of types.
 * @return List<Us...>() where Us... are unique types from Ts...
 */
template <
    template <typename...> typename List,
    typename... Ts
>
consteval auto deduplicate_type_list()
{
    constexpr auto op_accumulate = []<std::size_t CurTIndex, typename CurT>(
        auto &&list) consteval
    {
        return concatenate_lists(
            List<CurT>(),
            std::forward<decltype(list)>(list)
        );
    };
    /*
     * todo: bug somehow clang 20.0.0 has some problems regarding
     *   variables initialized with calc_unique_type_array<Ts...>().
     *   the problem seems to root from how MSVC's stdlib handle
     *   std::string's internal memory allocation in constexpr contexts.
     *   so we have to use them as some template argument to avoid
     *   memory allocation. maybe P2747R2 constexpr placement new would
     *   fix that problem. the current workaround is to put the expression
     *   in an unevaluated context so it doesn't allocate memory.
     */
    constexpr auto op_cond_keep_cur_t = []<
        std::size_t CurTIndex,
        bool IsCurTUnique = calc_unique_type_array<Ts...>()[CurTIndex] == '1'
    > consteval
    {
        return IsCurTUnique;
    };
    return reduce<Ts...>(List<>(), op_accumulate, op_cond_keep_cur_t);
}

/*
 * Static Tests
 */

// empty list
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<containers::TypeList>()),
    containers::TypeList<>
>);
// single type
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<containers::TypeList, int>()),
    containers::TypeList<int>
>);
// all the same
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<containers::TypeList, int, int, int>()),
    containers::TypeList<int>
>);
// identical types, also tests the order
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<containers::TypeList, int, bool>()),
    containers::TypeList<int, bool>
>);
// duplicated in the middle
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<containers::TypeList, int, int, bool>()),
    containers::TypeList<int, bool>
>);

template <typename... Lists>
consteval auto deduplicate_merge_lists(Lists...lists)
    requires (sizeof...(lists) > 1)
{
    constexpr auto concatenated = concatenate_lists(lists...);
    constexpr auto op = []<
        template <typename...> typename List,
        typename... Ts
    >(List<Ts...>)
    {
        return deduplicate_type_list<List, Ts...>();
    };
    return op(concatenated);
}

/*
 * Static Tests
 */

static_assert(std::is_same_v<
    decltype(deduplicate_merge_lists(
        containers::TypeList<int, char, bool>(),
        containers::TypeList<bool, char, float>()
    )),
    containers::TypeList<int, char, bool, float>
>);
static_assert(std::is_same_v<
    decltype(deduplicate_merge_lists(
        containers::TypeList<int, char, bool>(),
        containers::TypeList<bool, double, int>(),
        containers::TypeList<>()
    )),
    containers::TypeList<int, char, bool, double>
>);
}
