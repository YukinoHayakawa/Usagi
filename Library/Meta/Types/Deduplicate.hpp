#pragma once

/*
 *
 */

#include <type_traits>
#include <utility>
#include <bitset>
#include <algorithm>

#include <Usagi/Library/Meta/Types/List/TypeList.hpp>
#include <Usagi/Library/Meta/Types/List/Concatenate.hpp>

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
    if constexpr(sizeof...(Ts) == 0)
    {
        return false;
    }
    else
    {
        // using CollectedTypeListT =
            // AggregatedUniqueBaseClassList<TypeTag<Ts...[PrecedingIndices]>...>;
        // return false;
        // return CollectedTypeListT::template has_base_class<TypeTag<T>>();
        // return is_type_in_list<T>(CollectedTypeListT());
        // return std::is_base_of_v<TypeTag<T>, CollectedTypeListT>;
        // return __is_virtual_base_of(T, CollectedTypeListT);
        // return std::is_convertible_v<
            // const volatile CollectedTypeListT *,
            // const volatile T *
        // >;

        /*
         * Both the fold expression and std::disjunction can perform
         * short-circuit evaluation. However, the fold expression is more
         * straightforward, avoiding instantiating the std::disjunction
         * template until a true value is found. Since type list deduplication
         * is heavily used in the engine, this line of code might have
         * significant impact on the compilation time.
         * My own test shows using the template approach is about 1.15 times
         * slower than using the fold expression.
         */
        return (std::is_same_v<T, Ts...[PrecedingIndices]> || ...);
        // return std::disjunction_v<std::is_same<T, Ts...[PrecedingIndices]>...>;
    }
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

    /*
    // check whether each type is duplicated starting from the first one.
    map<Ts...>([&]<std::size_t CurIndex, typename CurT> {
        is_type_unique[CurIndex] = !details::is_type_duplicated<CurT, Ts...>(
            // make_index_sequence automatically creates a
            // sequence ending with I - 1.
            std::make_index_sequence<CurIndex>()
        );
    });
    */

    AggregatedUniqueBaseClassList<TypeTag<Ts>...>::eval_class_list(
        [&]<std::size_t CurIndex, typename CurT, bool Unique> {
            is_type_unique[CurIndex] = Unique;
        }
    );
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

/*
template <typename... Ts>
struct StaticUniqueTypeArray
{
    static constexpr auto UniqueTypeArray { calc_unique_type_array<Ts...>() };
};

template <typename... Ts>
constexpr std::string UniqueTypeArray = calc_unique_type_array<Ts...>();
*/


template <typename... Ts>
struct StaticUniqueTypeArray
{
    // avoid operator new
   /*constexpr static inline*/ char UniqueTypeArray[sizeof...(Ts)];
    constexpr StaticUniqueTypeArray()
    {
        const auto str = calc_unique_type_array<Ts...>();
        std::copy(str.data(), str.data() + sizeof...(Ts), UniqueTypeArray);
    }
};

template <>
struct StaticUniqueTypeArray<>
{
    // specialization preventing zero sized array
    /*constexpr static inline*/ char UniqueTypeArray[1] { };
    constexpr StaticUniqueTypeArray() = default;
};

template <typename... Ts>
constexpr static inline StaticUniqueTypeArray<Ts...> UniqueTypeArray;


template <typename... Ts>
struct StaticUniqueTypeArray2
{
   /*constexpr static inline*/ std::string UniqueTypeArray; /*{ calc_unique_type_array<Ts...>() };*/
    explicit constexpr StaticUniqueTypeArray2(std::string &&str)
    {
        UniqueTypeArray = std::move(str);
    }
};

// doesn't work
template <typename... Ts>
constexpr static inline StaticUniqueTypeArray2<Ts...> UniqueTypeArray2 { std::move(calc_unique_type_array<Ts...>()) };
}

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
     *   as a template parameter. don't know why this works.
     */
    // constexpr auto x = details::calc_unique_type_array<Ts...>();

    /*
    using IsCurTUniqueOpT = decltype([]<std::size_t Index> consteval {
        static auto array = details::calc_unique_type_array<Ts...>();
        return array[Index] == '1';
    });
    */

    //
    constexpr auto op_cond_keep_cur_t = []<
        std::size_t CurTIndex
    // ,
    //     // bool IsCurTUnique =
    //     // true &&
    //     //     // details::StaticUniqueTypeArray<Ts...>().
    //     //         details::UniqueTypeArray<Ts...>[CurTIndex] == '1'
    //     //     // details::calc_unique_type_array<Ts...>()[CurTIndex] == '1'
    //     typename IsCurTUnique = decltype([]<std::size_t Index> consteval -> bool {
    //         static auto array = details::calc_unique_type_array<Ts...>();
    //         return array[Index] == '1';
    //     })
    > consteval
    {
        // details::UniqueTypeArray<Ts...> is cached
        return details::UniqueTypeArray<Ts...>.UniqueTypeArray[CurTIndex] == '1';
        // return details::UniqueTypeArray2<Ts...>.UniqueTypeArray2[CurTIndex] == '1';
        // return IsCurTUnique();
    };

    // return reduce<Ts...>(List<>(), op_accumulate, op_cond_keep_cur_t.template operator()<>());
    return reduce<Ts...>(List<>(), op_accumulate, op_cond_keep_cur_t);
}

namespace tests
{
template <std::size_t I>
struct TypeListDeduplicateDummyPlaceholder { };

/*
 * used for testing compilation time
 */
template <std::size_t N>
consteval auto benchmark_deduplicate_n_unique_types()
{
    constexpr auto op = []<typename Int, Int... Is>
    (std::integer_sequence<Int, Is...>) consteval
    {
        return std::is_same_v<
            TypeList<TypeListDeduplicateDummyPlaceholder<Is>...>,
            decltype(deduplicate_type_list<
                TypeList,
                TypeListDeduplicateDummyPlaceholder<Is>...
            >())
        >;
    };
    return op(std::make_index_sequence<N>());
}

static_assert(benchmark_deduplicate_n_unique_types<100>());
}

/*
 * Static Tests
 */

// empty list
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<TypeList>()),
    TypeList<>
>);
// single type
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<TypeList, int>()),
    TypeList<int>
>);
// all the same
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<TypeList, int, int, int>()),
    TypeList<int>
>);
// identical types, also tests the order
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<TypeList, int, bool>()),
    TypeList<int, bool>
>);
// duplicated in the middle
static_assert(std::is_same_v<
    decltype(deduplicate_type_list<TypeList, int, int, bool>()),
    TypeList<int, bool>
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
        TypeList<int, char, bool>(),
        TypeList<bool, char, float>()
    )),
    TypeList<int, char, bool, float>
>);
static_assert(std::is_same_v<
    decltype(deduplicate_merge_lists(
        TypeList<int, char, bool>(),
        TypeList<bool, double, int>(),
        TypeList<>()
    )),
    TypeList<int, char, bool, double>
>);
}
