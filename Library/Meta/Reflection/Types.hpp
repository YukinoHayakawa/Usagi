#pragma once

#include <algorithm>
#include <ranges>

#include "StaticReflection.hpp"
#include "Statics.hpp"

namespace usagi::meta::reflection
{
template <std::meta::info SortFunc, typename... Ts>
    requires(std::meta::is_function(SortFunc))
consteval auto sorted_types()
{
    std::array<std::meta::info, sizeof...(Ts)> sorted_types { ^^Ts... };
    std::ranges::sort(sorted_types, std::ranges::less {}, [](auto && refl) {
        return [:SortFunc:](refl);
    });
    return sorted_types;
}

template <typename... Ts>
consteval auto sorted_types_by_names()
{
    return sorted_types<^^std::meta::display_string_of, Ts...>();
}

template <typename... Ts>
consteval auto unique_type_list_sorted_by_names()
{
    // todo: bug - unique somehow returns an empty subrange, so we gotta do it
    //   ourselves.
    // const auto unique_range = std::ranges::unique(sorted_types);
    // return std::define_static_array(unique_range);

    constexpr auto sorted_types_ = sorted_types_by_names<Ts...>();
    std::array<std::meta::info, sizeof...(Ts)> unique_types {};
    std::size_t                                num_unique_types = 0;
    std::size_t                                i                = 0;
    template for(constexpr auto t : sorted_types_)
    {
        // push the first which is always unique.
        if(num_unique_types == 0)
        {
            unique_types[num_unique_types++] = sorted_types_[i];
        }
        else
        {
            if(unique_types[num_unique_types - 1] != sorted_types_[i + 1])
            {
                unique_types[num_unique_types++] = sorted_types_[++i];
            }
            else
            {
                ++i;
            }
        }
    }
    return make_static_subarray(unique_types, num_unique_types);
}

template <typename T, typename... Ts>
consteval bool is_type_in_list()
{
    constexpr std::array types { ^^Ts... };
    return std::ranges::find(types, ^^T) != types.end();
}
} // namespace usagi::meta::reflection
