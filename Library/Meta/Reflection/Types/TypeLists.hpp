#pragma once

#include <algorithm>
#include <initializer_list>
#include <ranges>

#include <Usagi/Library/Algorithms/Ranges.hpp>
#include <Usagi/Library/Meta/Reflection/Utilities/MakeStatics.hpp>

#include "Types.hpp"

namespace usagi::meta::reflection
{
/*
 * Make an `std::array` of dealiased type reflections. Usually used in
 * `consteval` contexts as `std::array` is more `constexpr`-compatible.
 */
template <typename... Ts>
consteval auto make_type_list_array()
{
    static constexpr std::array<std::meta::info, sizeof...(Ts)> type_list {
        make_explicit_dealias<Ts>()...
    };
    return type_list;
}

/*
 * Get an iterable list of dealiased type reflections.
 */
template <typename... Ts>
consteval auto & static_type_list()
{
    static constexpr std::initializer_list<std::meta::info> type_list {
        make_explicit_dealias<Ts>()...
    };
    return type_list;
}

/*
 * Sort a list of types given a projection function.
 */
template <std::meta::info ProjFunc, typename... Ts>
    requires (std::meta::is_function(ProjFunc))
consteval auto sort_types()
{
    auto sorted_types = make_type_list_array<Ts...>();
    std::ranges::sort(sorted_types, std::ranges::less { }, [](auto && refl) {
        return [:ProjFunc:](refl);
    });
    return sorted_types;
}

template <typename... Ts>
consteval auto sort_types_by_names()
{
    return sort_types<^^std::meta::display_string_of, Ts...>();
}

template <typename... Ts>
consteval auto unique_types_sorted_by_names()
{
    // todo: bug - unique somehow returns an empty subrange, so we gotta do it
    //   ourselves.
    // const auto unique_range = std::ranges::unique(sorted_types);
    // return std::define_static_array(unique_range);

    constexpr auto sorted_types_ = sort_types_by_names<Ts...>();
    constexpr auto unique_types  = ranges::unique(sorted_types_);

    return make_static_subarray(unique_types.first, unique_types.second);
}

template <typename T, typename... Ts>
consteval bool is_type_in_list()
{
    constexpr auto types = make_type_list_array<Ts...>();
    return std::ranges::find(types, ^^T) != types.end();
}

namespace static_tests
{
consteval
{
    // type lists
    constexpr auto   list_a = make_type_list_array<int, char, bool, bool>();
    constexpr auto & list_b = static_type_list<int, char, bool, bool>();
    static_assert(ranges::is_equal(list_a, list_b));

    // type list sorting
    constexpr static auto sorted_type_list =
        make_type_list_array<bool, bool, char, int>();
    // clang-format off
    static_assert(ranges::is_equal(
        [:std::meta::substitute(^^sort_types_by_names, list_a):](),
        sorted_type_list
    ));
    // clang-format on

    // unique types
    constexpr static auto unique_type_list =
        make_type_list_array<bool, char, int>();
    // clang-format off
    static_assert(ranges::is_equal(
        [:std::meta::substitute(^^unique_types_sorted_by_names, list_a):](),
        unique_type_list
    ));
    // clang-format on

    static_assert(is_type_in_list<int, int, bool>());
    static_assert(is_type_in_list<int, int>());
    static_assert(!is_type_in_list<int>());
    static_assert(!is_type_in_list<int, bool>());
}
} // namespace static_tests
} // namespace usagi::meta::reflection
